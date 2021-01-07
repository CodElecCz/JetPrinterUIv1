#include "stdafx.h"
#include "NodeByteArray.h"
#include "NodeByte.h"

namespace Utilities
{
	namespace Node
	{
		NodeByteArray::NodeByteArray(string Id, NodeType Type, NodeAccess Access, UINT16 StartingAddress, UINT16 RequestedCount)
			: m_Address(StartingAddress), m_Count(RequestedCount), m_SerialDevId(49), INode(Id, Type, Access), m_rValueInit(true)
		{
			switch(Type)
			{
			case NodeType_ByteArray:
				m_ReadData.resize(m_Count);
				m_WriteData.resize(m_Count);
				break;
			default:
				throw Exc(this->Id(), "Not supported area type");
			}
			m_ReadFlag.resize(m_Count);
			m_WriteFlag.resize(m_Count);

			m_ReadTime.resize(m_Count);
			m_WriteTime.resize(m_Count);

			memset(&m_ReadTimeGlobal, 0, sizeof(SYSTEMTIME));
			memset(&m_WriteTimeGlobal, 0, sizeof(SYSTEMTIME));
		}

		NodeByteArray::~NodeByteArray(void)
		{
			;
		}

		void NodeByteArray::Refresh(void)
		{
			map<string, INode::INodePtr>::iterator it = m_Nodes.begin();
			while(it!=m_Nodes.end())
			{
				it->second->Refresh();
				it++;
			}
		}

		void NodeByteArray::SetReadArea(UINT8 ReadData[], UINT32 ReadDataSize)
		{
			CAutoLock al(m_ReadLock);

			int CopySize = (ReadDataSize>m_Count)? m_Count:ReadDataSize;
			int cmp = memcmp(m_ReadData.data(), ReadData, CopySize*sizeof(UINT8));
			memcpy(m_ReadData.data(), ReadData, CopySize*sizeof(UINT8));

			//clear flag
			memset(m_ReadFlag.data(), 1, m_Count*sizeof(UINT16));

			if(m_rValueInit)
			{
				m_rValueInit = false;
				Refresh();
			}
			else if(cmp!=0)
			{
				OnValueChanged(0);
				Refresh();
			}
		}
			
		void NodeByteArray::SetWriteArea(UINT8 WriteData[], UINT32 WriteDataSize)
		{
			CAutoLock al(m_WriteLock);

			int CopySize = (WriteDataSize>m_Count)? m_Count:WriteDataSize;
			int cmp = memcmp(m_WriteData.data(), WriteData, CopySize*sizeof(UINT8));
			memcpy(m_WriteData.data(), WriteData, CopySize*sizeof(UINT8));

			//clear flag
			memset(m_WriteFlag.data(), 0, m_Count*sizeof(UINT16));
		}

		void NodeByteArray::Read(UINT8 ReadData[], PUINT32 ReadDataSize)
		{
			UINT32 allocReadDataSize = *ReadDataSize;
			*ReadDataSize = 0;

			CAutoLock al(m_ReadLock);

			int CopySize = (allocReadDataSize>m_Count)? m_Count:allocReadDataSize;
			memcpy(ReadData, m_ReadData.data(), CopySize*sizeof(UINT8));
			*ReadDataSize = CopySize;
		}

		void NodeByteArray::GetWriteArea(UINT8 WriteData[], UINT16 WriteFlag[], PUINT32 WriteDataSize)
		{
			CAutoLock al(m_WriteLock);

			int CopySize = (*WriteDataSize>m_Count)? m_Count:*WriteDataSize;
			*WriteDataSize = 0;

			memcpy(WriteData, m_WriteData.data(), CopySize*sizeof(UINT8));
			memcpy(WriteFlag, m_WriteFlag.data(), CopySize*sizeof(UINT16));

			//memset(m_WriteFlag.get(), 0, CopySize*sizeof(bool));
			*WriteDataSize = CopySize;
		}

		void NodeByteArray::SetWriteAreaAcknoledge(UINT16 Offset, UINT32 WriteDataSize)
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
		}

		INode* NodeByteArray::CreateNode(const char* Id, NodeType Type, UINT16 Offset)
		{
			return CreateNode(Id, Type, Offset, m_Access);
		}

		INode* NodeByteArray::CreateNode(const char* Id, NodeType Type, UINT16 Offset, NodeAccess Access)
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
			case NodeType_Byte:
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
		

			NodeByte::NodeBytePtr node(new NodeByte(id,
													Type,
													nodeAccess,
													offset,
													m_ReadData[wOffset],
													m_WriteData[wOffset],
													m_ReadFlag[wOffset],
													m_WriteFlag[wOffset],
													m_ReadLock,
													m_WriteLock,
													m_ReadTime[wOffset],
													m_WriteTime[wOffset],
													wOffset));

			m_Nodes[id] = node;

			OnValueChanged(0);

			return dynamic_cast<INode*>(node.get());
		}
	}
}
