#pragma once

#include "stdafx.h"
#include "Def.h"

using namespace std;
using namespace Utilities;

namespace Utilities
{
	namespace Node
	{
		class LIB_API NodeByteShared
		{
		public:
			NodeByteShared(UINT8& ReadVar, UINT8& WriteVar, UINT16& ReadFlag, UINT16& WriteFlag, CLock& ReadLock, CLock& WriteLock, SYSTEMTIME& ReadTimeStamp, SYSTEMTIME& WriteTimeStamp)
				: m_rValue(ReadVar), m_wValue(WriteVar), m_rFlag(ReadFlag), m_wFlag(WriteFlag), m_ReadLock(ReadLock), m_WriteLock(WriteLock), m_ReadTime(ReadTimeStamp), m_WriteTime(WriteTimeStamp)
			{ };
			
			virtual ~NodeByteShared(void) {};

		protected:
			//lock
			CLock& m_ReadLock;
			CLock& m_WriteLock;

			//value
			UINT8& m_rValue;
			UINT8& m_wValue;
			
			//flag
			UINT16& m_rFlag;
			UINT16& m_wFlag;

			//timestamp
			SYSTEMTIME& m_ReadTime;
			SYSTEMTIME& m_WriteTime;
		};

		class LIB_API NodeByte : public INode, private NodeByteShared
		{
		public:
			NodeByte(string Id, NodeType Type, NodeAccess Access, UINT16 Offset, UINT8& ReadVar, UINT8& WriteVar, UINT16& ReadFlag, UINT16& WriteFlag, CLock& ReadLock, CLock& WriteLock, SYSTEMTIME& ReadTimeStamp, SYSTEMTIME& WriteTimeStamp, UINT32 DataOffset = 0)
				: m_Offset(Offset), INode(Id, Type, Access, DataOffset), NodeByteShared(ReadVar, WriteVar, ReadFlag, WriteFlag, ReadLock, WriteLock, ReadTimeStamp, WriteTimeStamp), m_rValueInit(true)
			{};
			
			~NodeByte(void)
			{};

		public:
			void Read(bool* val);
			void Read(PUINT8 val) { CAutoLock al(m_ReadLock); if (val) *val = m_rValue; m_rFlag = 0; };

			void Write(bool val);
			void Write(UINT8 val) { CAutoLock al(m_WriteLock); m_wValue = val; m_wFlag++; };

			void Write(bool* val);
			void Write(PUINT8 val) { CAutoLock al(m_WriteLock); if (val) *val = m_wValue; };

			bool ReadDataAvailable() { CAutoLock al(m_ReadLock); return (m_rFlag>0); };
			bool WriteDataAcknowledge() { CAutoLock al(m_WriteLock); return (m_wFlag==0); };
			
			//nodes
			INode* CreateNode(const char* Id, NodeType Type, UINT16 Offset);
			INode* CreateNode(const char* Id, NodeType Type, UINT16 Offset, NodeAccess Access);
			void Refresh(void);
			
			//timestamp
			SYSTEMTIME ReadTimeStamp() { CAutoLock al(m_ReadLock); return m_ReadTime; }
			SYSTEMTIME WriteTimeStamp() { CAutoLock al(m_WriteLock); return m_WriteTime; }

			void DelayTimer(UINT32 DelayON_MS, UINT32 DelayOFF_MS);

		private:
			NodeByte* GetNode(const char* Id);
			void Read(UINT8_UT Value, UINT16 offset, bool* val);
			void SetTimerDefaultVal(UINT8 val);

		public:
			typedef std::shared_ptr<NodeByte> NodeBytePtr;
			
		private:
			//OnNodeValueChanged
			UINT16 m_rValueLast;			
			bool m_rValueInit;

			UINT16 m_Offset;

#pragma warning(push)
#pragma warning(disable:4251)
			//DelayTimer
			Timer m_Timer;
#pragma warning(pop)
		};
	}
}
