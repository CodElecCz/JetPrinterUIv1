#include "stdafx.h"
#include "NodeBitArray.h"
#include "NodeBit.h"

namespace Utilities
{
	namespace Node
	{
		NodeBitArray::NodeBitArray(string Id, NodeType Type, NodeAccess Access, UINT16 StartingAddress, UINT16 RequestedCount, UINT16 CustomType)
			: m_Address(StartingAddress), m_Count(RequestedCount), m_SerialDevId(49), m_CustomType(CustomType), INode(Id, Type, Access), m_rValueInit(true),
			m_ReadData(nullptr),  m_WriteData(nullptr)
		{
			switch(Type)
			{
			case NodeType_BitArray:
				m_ReadData = new bool[m_Count];
				memset(m_ReadData, 0, m_Count*sizeof(bool));

				m_WriteData = new bool[m_Count];
				memset(m_WriteData, 0, m_Count*sizeof(bool));
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
		};

		NodeBitArray::~NodeBitArray(void)
		{
			if(m_ReadData!=nullptr)
				delete[] m_ReadData;

			if(m_WriteData!=nullptr)
				delete[] m_WriteData;
		};

		void NodeBitArray::Refresh(void)
		{
			map<string, INode::INodePtr>::iterator it = m_Nodes.begin();
			while(it!=m_Nodes.end())
			{
				it->second->Refresh();
				it++;
			}
		}

		void NodeBitArray::SetReadArea(bool ReadData[], UINT32 ReadDataSize)
		{
			CAutoLock al(m_ReadLock);

			int CopySize = (ReadDataSize>m_Count)? m_Count:ReadDataSize;
			int cmp = memcmp(m_ReadData, ReadData, CopySize*sizeof(bool));
			memcpy(m_ReadData, ReadData, CopySize*sizeof(bool));
			
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

		void NodeBitArray::SetWriteArea(bool WriteData[], UINT32 WriteDataSize)
		{
			CAutoLock al(m_WriteLock);

			int CopySize = (WriteDataSize>m_Count)? m_Count:WriteDataSize;
			int cmp = memcmp(m_WriteData, WriteData, CopySize*sizeof(bool));
			memcpy(m_WriteData, WriteData, CopySize*sizeof(bool));
			
			//clear flag
			memset(m_WriteFlag.data(), 0, m_Count*sizeof(UINT16));
		}

		void NodeBitArray::Write(UINT16 Offset, UINT16 WriteData[], UINT32 WriteDataSize)
		{
			if(Offset>m_Count)
				throw Exc(this->Id(), "Offset out of range");

			CAutoLock al(m_WriteLock);

			UINT32 nbOfWordToCopy = ((Offset+WriteDataSize)>m_Count)? (m_Count-Offset):WriteDataSize;
			memcpy(m_WriteData+Offset, WriteData, nbOfWordToCopy*sizeof(UINT16));
			memset(m_WriteFlag.data()+Offset, 1, nbOfWordToCopy*sizeof(UINT16));
		}

		void NodeBitArray::GetWriteArea(bool WriteData[], UINT16 WriteFlag[], PUINT32 WriteDataSize)
		{
			CAutoLock al(m_WriteLock);

			int CopySize = (*WriteDataSize>m_Count)? m_Count:*WriteDataSize;
			*WriteDataSize = 0;

			memcpy(WriteData, m_WriteData, CopySize*sizeof(bool));
			memcpy(WriteFlag, m_WriteFlag.data(), CopySize*sizeof(UINT16));

			//memset(m_WriteFlag.get(), 0, CopySize*sizeof(bool));
			*WriteDataSize = CopySize;
		}

		void NodeBitArray::SetWriteAreaAcknoledge(UINT16 Offset, UINT32 WriteDataSize)
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

		INode* NodeBitArray::CreateNode(const char* Id, NodeType Type, UINT16 Offset)
		{
			return CreateNode(Id, Type, Offset, m_Access);
		}

		INode* NodeBitArray::CreateNode(const char* Id, NodeType Type, UINT16 Offset, NodeAccess Access)
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

			switch(Type)
			{
			case NodeType_Bit:
				{
					if(Offset>=m_Count)
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
					sprintf_s(msg, sizeof(msg), "Not supported node type for coil area, node name '%s'", Id);
					throw Exc(this->Id(), msg);
				}
			}
				
			NodeBit::NodeBitPtr node(new NodeBit(id,
												Type,
												nodeAccess,
												m_ReadData[Offset],
												m_WriteData[Offset],
												m_ReadFlag[Offset],
												m_WriteFlag[Offset],
												m_ReadLock,
												m_WriteLock,
												m_ReadTime[Offset],
												m_WriteTime[Offset],
												Offset));

			m_Nodes[id] = node;

			return dynamic_cast<INode*>(node.get());
		}
	}
}
