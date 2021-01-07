#pragma once

#include "stdafx.h"
#include "Def.h"

using namespace std;
using namespace Utilities;

namespace Utilities
{
	namespace Node
	{
		class LIB_API NodeByteArray : public INode
		{
		public:
			NodeByteArray(string Id, NodeType Type, NodeAccess Access, UINT16 StartingAddress, UINT16 RequestedCount);
			~NodeByteArray(void);

		public:
			//area
			void SetReadArea(UINT8 ReadData[], UINT32 ReadDataSize);
			void SetWriteArea(UINT8 WriteData[], UINT32 WriteDataSize);
			void GetWriteArea(UINT8 WriteData[], UINT16 WriteFlag[], PUINT32 WriteDataSize);
			void SetWriteAreaAcknoledge(UINT16 Offset, UINT32 WriteDataSize);
			void Read(UINT8 ReadData[], PUINT32 ReadDataSize);

			//nodes
			INode* CreateNode(const char* Id, NodeType Type, UINT16 Offset);
			INode* CreateNode(const char* Id, NodeType Type, UINT16 Offset, NodeAccess Access);
			void Refresh(void);
			
			//timestamp
			SYSTEMTIME ReadTimeStamp() { CAutoLock al(m_ReadLock); return m_ReadTimeGlobal; }
			SYSTEMTIME WriteTimeStamp() { CAutoLock al(m_WriteLock); return m_WriteTimeGlobal; }

			//properties
			UINT16 Address() const { return m_Address; }
			UINT16 Count() const { return m_Count; }
			void SetSerialDevId(UINT8 DevId) { m_SerialDevId = DevId; }
			UINT8 GetSerialDevId() { return m_SerialDevId; }

		private:
			//timestamp
			void SetReadTimeStamp() { GetSystemTime(&m_ReadTimeGlobal); }
			void SetWriteTimeStamp() { GetSystemTime(&m_WriteTimeGlobal); }
			void SetReadTimeStamp(UINT16 Offset) { GetSystemTime(&m_ReadTimeGlobal); m_ReadTime[Offset] = m_ReadTimeGlobal; }
			void SetWriteTimeStamp(UINT16 Offset) { GetSystemTime(&m_WriteTimeGlobal); m_WriteTime[Offset] = m_WriteTimeGlobal; }

		public:
			typedef shared_ptr<NodeByteArray> NodeByteArrayPtr;

		private:
#pragma warning(push)
#pragma warning(disable:4251)
			//register
			vector<UINT8> m_ReadData;
			vector<UINT8> m_WriteData;
			
			//flag
			vector<UINT16> m_ReadFlag;
			vector<UINT16> m_WriteFlag;

			//lock
			CLock m_ReadLock;
			CLock m_WriteLock;

			UINT8 m_SerialDevId;

			//timestamp
			SYSTEMTIME m_ReadTimeGlobal;
			SYSTEMTIME m_WriteTimeGlobal;
			vector<SYSTEMTIME> m_ReadTime;
			vector<SYSTEMTIME> m_WriteTime;
#pragma warning(pop)
			bool m_rValueInit;

			//area
			UINT16 m_Address;
			UINT16 m_Count;
		};
	}
}

