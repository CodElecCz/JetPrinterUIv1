#pragma once

#include "stdafx.h"
#include "Def.h"

using namespace std;
using namespace Utilities;

namespace Utilities
{
	namespace Node
	{
		class LIB_API NodeBitArray : public INode
		{
		public:
			NodeBitArray(string Id, NodeType Type, NodeAccess Access, UINT16 StartingAddress, UINT16 RequestedCount, UINT16 CustomType = 0);
			~NodeBitArray(void);

		public:		
			//area
			void SetReadArea(bool ReadData[], UINT32 ReadDataSize);
			void SetWriteArea(bool WriteData[], UINT32 WriteDataSize);
			void GetWriteArea(bool WriteData[], UINT16 WriteFlag[], PUINT32 WriteDataSize);
			void SetWriteAreaAcknoledge(UINT16 Offset, UINT32 WriteDataSize);
			void Write(UINT16 Offset, UINT16 WriteData[], UINT32 WriteDataSize);

			//nodes
			INode* CreateNode(const char* Id, NodeType Type, UINT16 Offset);
			INode* CreateNode(const char* Id, NodeType Type, UINT16 Offset, NodeAccess Access);
			void Refresh(void);

			//timestmp
			SYSTEMTIME ReadTimeStamp() { CAutoLock al(m_ReadLock); return m_ReadTimeGlobal; }
			SYSTEMTIME WriteTimeStamp() { CAutoLock al(m_WriteLock); return m_WriteTimeGlobal; }

			//properties
			UINT16 Address() const { return m_Address; }
			UINT16 Count() const { return m_Count; }
			UINT16 CustomType() const { return m_CustomType; }
			void SetSerialDevId(UINT8 DevId) { m_SerialDevId = DevId; }
			UINT8 GetSerialDevId() { return m_SerialDevId; }
		
		private:
			//timestamp
			void SetReadTimeStamp() { GetSystemTime(&m_ReadTimeGlobal); }
			void SetWriteTimeStamp() { GetSystemTime(&m_WriteTimeGlobal); }
			void SetReadTimeStamp(UINT16 Offset) { GetSystemTime(&m_ReadTimeGlobal); m_ReadTime[Offset] = m_ReadTimeGlobal; }
			void SetWriteTimeStamp(UINT16 Offset) { GetSystemTime(&m_WriteTimeGlobal); m_WriteTime[Offset] = m_WriteTimeGlobal; }

		public:
			typedef shared_ptr<NodeBitArray> NodeBitArrayPtr;

		private:
#pragma warning(push)
#pragma warning(disable:4251)
			//coil
			bool* m_ReadData;
			bool* m_WriteData;
			
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
			UINT16 m_CustomType;
		};
	}
}

