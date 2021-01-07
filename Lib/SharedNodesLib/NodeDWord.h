#pragma once

#include "stdafx.h"
#include "Def.h"

using namespace std;
using namespace Utilities;

namespace Utilities
{
	namespace Node
	{
		class LIB_API NodeDWordShared
		{
		public:
			NodeDWordShared(UINT32& ReadVar, UINT32& WriteVar, UINT16& ReadFlag, UINT16& WriteFlag, CLock& ReadLock, CLock& WriteLock, SYSTEMTIME& ReadTimeStamp, SYSTEMTIME& WriteTimeStamp, UINT64& ReadCnt, UINT64& WriteCnt)
				: m_rValue(ReadVar), 
				m_wValue(WriteVar), 
				m_rFlag(ReadFlag), 
				m_wFlag(WriteFlag), 
				m_rLock(ReadLock), 
				m_wLock(WriteLock), 
				m_rTime(ReadTimeStamp), 
				m_wTime(WriteTimeStamp),
				m_rCounter(ReadCnt),
				m_wCounter(WriteCnt)
			{ };
			
			virtual ~NodeDWordShared(void) {};

		protected:
			//lock
			CLock& m_rLock;
			CLock& m_wLock;

			//value
			UINT32& m_rValue;
			UINT32& m_wValue;
			
			//flag
			UINT16& m_rFlag;
			UINT16& m_wFlag;

			//counter
			UINT64& m_rCounter;
			UINT64& m_wCounter;

			//timestamp
			SYSTEMTIME& m_rTime;
			SYSTEMTIME& m_wTime;
		};

		class LIB_API NodeDWord : public INode, private NodeDWordShared
		{
		public:
			NodeDWord(string Id, NodeType Type, NodeAccess Access, UINT16 Offset, UINT32& ReadVar, UINT32& WriteVar, UINT16& ReadFlag, UINT16& WriteFlag, CLock& ReadLock, CLock& WriteLock, SYSTEMTIME& ReadTimeStamp, SYSTEMTIME& WriteTimeStamp, UINT64& ReadCounter, UINT64& WriteCounter, UINT32 DataOffset = 0)
				: m_Offset(Offset), 
				m_rValueInit(true),
				m_rTick(0),
				m_rCnt(0),
				INode(Id, Type, Access, DataOffset), 
				NodeDWordShared(ReadVar, WriteVar, ReadFlag, WriteFlag, ReadLock, WriteLock, ReadTimeStamp, WriteTimeStamp, ReadCounter, WriteCounter)
			{};
			
			~NodeDWord(void)
			{};

		public:
			void Read(bool* val);
			void Read(PUINT8 val);
			void Read(PUINT16 val);
			void Read(PUINT32 val) { CAutoLock al(m_rLock); if(val) *val = m_rValue; m_rFlag = 0; };
			void Read(float* val) { CAutoLock al(m_rLock); UINT32_UT Value; Value.dword = m_rValue; if (val) *val =Value.real32; m_rFlag = 0; };

			void Write(bool val);
			void Write(UINT8 val);
			void Write(UINT16 val);
			void Write(UINT32 val) { CAutoLock al(m_wLock); m_wValue = val; m_wFlag++; };
			void Write(float val) { CAutoLock al(m_wLock); UINT32_UT Value; Value.real32 = val; m_wValue = Value.dword; m_wFlag++; };

			void Write(bool* val);
			void Write(PUINT8 val);
			void Write(PUINT16 val);
			void Write(PUINT32 val) { CAutoLock al(m_wLock); if(val) *val = m_wValue; };
			void Write(float* val) { CAutoLock al(m_wLock); UINT32_UT Value; Value.dword = m_wValue;  if (val) *val = Value.real32; };

			bool ReadDataAvailable() { CAutoLock al(m_rLock); return (m_rFlag>0); };
			bool WriteDataAcknowledge() { CAutoLock al(m_wLock); return (m_wFlag==0); };
			
			//nodes
			INode* CreateNode(const char* Id, NodeType Type, UINT16 Offset);
			INode* CreateNode(const char* Id, NodeType Type, UINT16 Offset, NodeAccess Access);
			void Refresh(void);

			//timestamp
			SYSTEMTIME ReadTimeStamp() { CAutoLock al(m_rLock); return m_rTime; }
			SYSTEMTIME WriteTimeStamp() { CAutoLock al(m_wLock); return m_wTime; }

			//counter
			UINT64 ReadCounter() { CAutoLock al(m_rLock); return m_rCounter; }
			UINT64 WriteCounter() { CAutoLock al(m_wLock); return m_wCounter; }
			double GetNodeReadRate();

			void DelayTimer(UINT32 DelayON_MS, UINT32 DelayOFF_MS);

		private:
			NodeDWord* GetNode(const char* Id);
			void Read(UINT32_UT Value, UINT16 offset, bool* val);
			void Read(UINT32_UT Value, UINT16 offset, PUINT8 val);
			void Read(UINT32_UT Value, UINT16 offset, PUINT16 val);
			void SetTimerDefaultVal(UINT16 val);

		public:
			typedef std::shared_ptr<NodeDWord> NodeDWordPtr;
			
		private:
			//OnNodeValueChanged
			UINT32 m_rValueLast;			
			bool m_rValueInit;

			//node rate
			UINT64 m_rTick;
			UINT64 m_rCnt;
			double m_rRate;

			UINT16 m_Offset;

#pragma warning(push)
#pragma warning(disable:4251)
			//DelayTimer
			Timer m_Timer;
#pragma warning(pop)
		};
	}
}
