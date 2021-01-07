#include "stdafx.h"
#include "NodeUInt64Array.h"
#include "NodeUInt64.h"

namespace Utilities
{
	namespace Node
	{
		NodeUInt64Array::NodeUInt64Array(string Id, NodeType Type, NodeAccess Access, UINT16 StartingAddress, UINT16 RequestedCount)
			: m_Address(StartingAddress), m_Count(RequestedCount), m_SerialDevId(49), INode(Id, Type, Access), m_rValueInit(true), m_ReadCounterGlobal(0), m_WriteCounterGlobal(0)
		{
			switch(Type)
			{
			case NodeType_UInt64Array:
				m_ReadData.resize(m_Count);
				m_WriteData.resize(m_Count);
				break;
			default:
				throw Exc(this->Id(), "Not supported area type");
			}

			//flag
			m_ReadFlag.resize(m_Count);
			m_WriteFlag.resize(m_Count);

			//Counter
			m_ReadCounter.resize(m_Count);
			m_WriteCounter.resize(m_Count);
			
			//time stamp
			m_ReadTime.resize(m_Count);
			m_WriteTime.resize(m_Count);

			memset(&m_ReadTimeGlobal, 0, sizeof(SYSTEMTIME));
			memset(&m_WriteTimeGlobal, 0, sizeof(SYSTEMTIME));
		}

		NodeUInt64Array::~NodeUInt64Array(void)
		{
			;
		}

		void NodeUInt64Array::Refresh(void)
		{
			map<string, INode::INodePtr>::iterator it = m_Nodes.begin();
			while(it!=m_Nodes.end())
			{
				it->second->Refresh();
				it++;
			}
		}

		void NodeUInt64Array::SetReadArea(UINT64 ReadData[], UINT32 ReadDataSize, UINT32 Offset)
		{
			CAutoLock al(m_ReadLock);
			
			int cmp = 0;
			
			if (Offset == 0)
			{
				UINT32 CopySize = (ReadDataSize > m_Count) ? m_Count : ReadDataSize;
				cmp = memcmp(m_ReadData.data(), ReadData, CopySize * sizeof(UINT64));
				memcpy(m_ReadData.data(), ReadData, CopySize * sizeof(UINT64));

				//clear flag
				memset(m_ReadFlag.data(), 1, m_Count * sizeof(UINT16));

				//timestamp
				for (UINT32 i = 0; i < CopySize; i++)
				{
					SetReadTimeStamp(Offset + i);
					SetReadCounter(Offset + i);
				}
			}
			else
			{
				UINT32 CopySize = ((ReadDataSize+Offset) > m_Count) ? (m_Count-Offset) : ReadDataSize;
				cmp = memcmp(&m_ReadData.at(Offset), ReadData, CopySize * sizeof(UINT64));
				memcpy(&m_ReadData.at(Offset), ReadData, CopySize * sizeof(UINT64));

				//clear flag
				memset(&m_ReadFlag.at(Offset), 1, CopySize * sizeof(UINT16));

				//timestamp
				for (UINT32 i = 0; i < CopySize; i++)
				{
					SetReadTimeStamp(Offset+i);
					SetReadCounter(Offset + i);
				}
			}

			//event changes
			if (m_rValueInit)
			{
				m_rValueInit = false;
				Refresh();
			}
			else if (cmp != 0)
			{
				OnValueChanged(0);
				Refresh();
			}
		}
			
		void NodeUInt64Array::SetWriteArea(UINT64 WriteData[], UINT32 WriteDataSize, UINT32 Offset)
		{
			CAutoLock al(m_WriteLock);
			
			int cmp = 0;

			if (Offset == 0)
			{
				UINT32 CopySize = (WriteDataSize > m_Count) ? m_Count : WriteDataSize;
				cmp = memcmp(m_WriteData.data(), WriteData, CopySize * sizeof(UINT64));
				memcpy(m_WriteData.data(), WriteData, CopySize * sizeof(UINT64));

				//clear flag
				memset(m_WriteFlag.data(), 0, m_Count * sizeof(UINT16));

				//timestamp
				for (UINT32 i = 0; i < CopySize; i++)
				{
					SetWriteTimeStamp(Offset + i);
					SetWriteCounter(Offset + i);
				}
			}
			else
			{
				UINT32 CopySize = ((WriteDataSize + Offset) > m_Count) ? (m_Count - Offset) : WriteDataSize;
				cmp = memcmp(&m_WriteData.at(Offset), WriteData, CopySize * sizeof(UINT64));
				memcpy(&m_WriteData.at(Offset), WriteData, CopySize * sizeof(UINT64));

				//clear flag
				memset(&m_WriteFlag.at(Offset), 1, CopySize * sizeof(UINT16));

				//timestamp
				for (UINT32 i = 0; i < CopySize; i++)
				{
					SetWriteTimeStamp(Offset+i);
					SetWriteCounter(Offset + i);
				}
			}
		}

		void NodeUInt64Array::Read(UINT64 ReadData[], PUINT32 ReadDataSize)
		{
			UINT32 allocReadDataSize = *ReadDataSize;
			*ReadDataSize = 0;

			CAutoLock al(m_ReadLock);

			int CopySize = (allocReadDataSize>m_Count)? m_Count:allocReadDataSize;
			memcpy(ReadData, m_ReadData.data(), CopySize*sizeof(UINT64));
			*ReadDataSize = CopySize;
		}

		void NodeUInt64Array::GetWriteArea(UINT64 WriteData[], UINT16 WriteFlag[], PUINT32 WriteDataSize)
		{
			CAutoLock al(m_WriteLock);

			int CopySize = (*WriteDataSize>m_Count)? m_Count:*WriteDataSize;
			*WriteDataSize = 0;

			memcpy(WriteData, m_WriteData.data(), CopySize*sizeof(UINT64));
			memcpy(WriteFlag, m_WriteFlag.data(), CopySize*sizeof(UINT16));

			//memset(m_WriteFlag.get(), 0, CopySize*sizeof(bool));
			*WriteDataSize = CopySize;
		}

		void NodeUInt64Array::SetWriteAreaAcknoledge(UINT16 Offset, UINT32 WriteDataSize)
		{
			if(Offset>m_Count)
				throw Exc(this->Id(), "Offset out of range");

			CAutoLock al(m_WriteLock);

			UINT32 nbOfWordToSet = ((Offset+WriteDataSize)>m_Count)? (m_Count-Offset):WriteDataSize;
#if 0
			memset(m_WriteFlag.get()+Offset, 0, nbOfWordToSet*sizeof(UINT16));
#else
			for(UINT32 i = Offset; i<(Offset+nbOfWordToSet); i++)
			{
				m_WriteFlag[i] = (m_WriteFlag[i]>1)? 1:0;
			}
#endif
			//timestamp
			for (UINT32 i = Offset; i<(Offset + nbOfWordToSet); i++)
			{
				//timestamp
				SetWriteTimeStamp(i);

				//counter
				SetWriteCounter(i);
			}
		}

		INode* NodeUInt64Array::CreateNode(const char* Id, NodeType Type, UINT16 Offset)
		{
			return CreateNode(Id, Type, Offset, m_Access);
		}

		INode* NodeUInt64Array::CreateNode(const char* Id, NodeType Type, UINT16 Offset, NodeAccess Access)
		{
			string id(Id);

			//check if allready exist
			std::map<string, INode::INodePtr>::iterator it = m_Nodes.find(id);
			if(it != m_Nodes.end())
			{
				char msg[256];
				sprintf_s(msg, sizeof(msg), "Duplicate area id: '%s'", Id);
				throw Exc(this->Id(), msg);
			}

			//node access
			NodeAccess nodeAccess = m_Access;
			if((m_Access&NodeAccess_Read)&&(m_Access&NodeAccess_Write))
			{
				nodeAccess = Access;
			}

			UINT16 wOffset = Offset;
			UINT16 offset = 0;

			switch(Type)
			{
			case NodeType_Bit:
				{
					if(Offset>=m_Count*64)
					{
						char msg[256];
						sprintf_s(msg, sizeof(msg), "Offset %d out of range (%d) for node '%s'", Offset, m_Count*64, Id);
						throw Exc(this->Id(), msg);
					}

					wOffset = Offset/64;
					offset = Offset%64;
				}
				break;
			case NodeType_Byte:
				{
					if(Offset>=m_Count*8)
					{
						char msg[256];
						sprintf_s(msg, sizeof(msg), "Offset %d out of range (%d) for node '%s'", Offset, m_Count*8, Id);
						throw Exc(this->Id(), msg);
					}

					wOffset = Offset/8;
					offset = Offset%8;
				}
				break;
			case NodeType_Word:
				{
					if(Offset>=m_Count*4)
					{
						char msg[256];
						sprintf_s(msg, sizeof(msg), "Offset %d out of range (%d) for node '%s'", Offset, m_Count*4, Id);
						throw Exc(this->Id(), msg);
					}

					wOffset = Offset / 4;
					offset = Offset % 4;
				}
				break;
			case NodeType_DWord:
			case NodeType_Real32:
				{
					if (Offset >= m_Count*2)
					{
						char msg[256];
						sprintf_s(msg, sizeof(msg), "Offset %d out of range (%d) for node '%s'", Offset, m_Count*2, Id);
						throw Exc(this->Id(), msg);
					}

					wOffset = Offset / 2;
					offset = Offset % 2;
				}
				break;
			case NodeType_UInt64:
			case NodeType_Real64:
				{
					if (Offset >= m_Count)
					{
						char msg[256];
						sprintf_s(msg, sizeof(msg), "Offset %d out of range (%d) for node '%s'", Offset, m_Count, Id);
						throw Exc(this->Id(), msg);
					}
				}
				break;
			default:
				{
					char msg[256];
					sprintf_s(msg, sizeof(msg), "Not supported node type: '%s'", NodeType_Str[Type]);
					throw Exc(this->Id(), msg);
				}
				break;
			}

			NodeUInt64::NodeUInt64Ptr node(new NodeUInt64(id,
														Type,
														nodeAccess,
														offset,
														m_ReadLock,
														m_WriteLock,
														m_ReadData[wOffset],
														m_WriteData[wOffset],
														m_ReadFlag[wOffset],
														m_WriteFlag[wOffset],														
														m_ReadTime[wOffset],
														m_WriteTime[wOffset], 
														m_ReadCounter[wOffset],
														m_WriteCounter[wOffset],
														wOffset));

			m_Nodes[id] = node;
		
			OnValueChanged(0);

			return dynamic_cast<INode*>(this);
		}
	}
}
