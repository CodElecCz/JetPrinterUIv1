#pragma once

#include "stdafx.h"
#include "Def.h"

using namespace std;
using namespace Utilities;

namespace Utilities
{
	namespace Node
	{
		class LIB_API NodeUInt64Array : public INode
		{
		public:
			NodeUInt64Array(string Id, NodeType Type, NodeAccess Access, UINT16 StartingAddress, UINT16 RequestedCount);
			~NodeUInt64Array(void);

		public:
			//area
			void SetReadArea(UINT64 ReadData[], UINT32 ReadDataSize, UINT32 Offset = 0);
			void SetWriteArea(UINT64 WriteData[], UINT32 WriteDataSize, UINT32 Offset = 0);
			void GetWriteArea(UINT64 WriteData[], UINT16 WriteFlag[], PUINT32 WriteDataSize);
			void SetWriteAreaAcknoledge(UINT16 Offset, UINT32 WriteDataSize);
			void Read(UINT64 ReadData[], PUINT32 ReadDataSize);

			//nodes
			INode* CreateNode(const char* Id, NodeType Type, UINT16 Offset);
			INode* CreateNode(const char* Id, NodeType Type, UINT16 Offset, NodeAccess Access);
			void Refresh(void);

			//timestamp
			SYSTEMTIME ReadTimeStamp() { CAutoLock al(m_ReadLock); return m_ReadTimeGlobal; }
			SYSTEMTIME WriteTimeStamp() { CAutoLock al(m_WriteLock); return m_WriteTimeGlobal; }

			//counter
			UINT64 ReadCounter() { CAutoLock al(m_ReadLock); return m_ReadCounterGlobal; }
			UINT64 WriteCounter() { CAutoLock al(m_WriteLock); return m_WriteCounterGlobal; }

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
			void SetReadCounter(UINT16 Offset) { m_ReadCounterGlobal++; m_ReadCounter[Offset]++; }
			void SetWriteCounter(UINT16 Offset) { m_WriteCounterGlobal++; m_WriteCounter[Offset]++; }

		public:
			typedef shared_ptr<NodeUInt64Array> NodeUInt64ArrayPtr;

		private:
#pragma warning(push)
#pragma warning(disable:4251)
			//register
			vector<UINT64> m_ReadData;
			vector<UINT64> m_WriteData;
			
			//flag
			vector<UINT16> m_ReadFlag;
			vector<UINT16> m_WriteFlag;

			//counter
			UINT64 m_ReadCounterGlobal;
			UINT64 m_WriteCounterGlobal;
			vector<UINT64> m_ReadCounter;
			vector<UINT64> m_WriteCounter;

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

