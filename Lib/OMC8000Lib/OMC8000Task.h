#pragma once

#include "stdafx.h"
#include "OMC8000Protocol.h"

using namespace std;
using namespace Utilities::Node;

namespace Utilities
{
	namespace OMC8000
	{
		class LIB_API OMC8000Task : public INode
		{
		public:
			OMC8000Task(string Id);
			~OMC8000Task(void);

		public:
			//create task connection for reading over TCP
			void Create(const char* IPaddress, UINT32 LoopTimeMS);

			//create task connection for reading over UDP
			void Create(const char* IPaddress, UINT32 LoopTimeMS, UINT16 LocalPort, UINT16 RemotePort);

			//create task connection for reading over Serial
			void Create(const char* COMport, UINT32 LoopTimeMS, SerialProtocol::BaudRate Baudrate);
			
			//register task memory area for RW thread
			NodeDWordArray* AddArea(const char* Id, const char* Variable, NodeAccess Access = (NodeAccess)(NodeAccess_Read | NodeAccess_Write));

			list<INode*> SearchNodeInterface(const char* Id);
			void Refresh(void) {}
			SYSTEMTIME ReadTimeStamp() { CAutoLock al(m_Lock); return m_ReadTime; }
			SYSTEMTIME WriteTimeStamp() { CAutoLock al(m_Lock); return m_WriteTime; }

			//start-stop-clear reading thread
			void Start(bool WaitForTaskRunning = false);
			//start-stop-clear reading thread
			void Stop();
			//start-stop-clear reading thread
			void Clear();

			bool TaskIsRunning() { return m_TaskIsRunning; };
			void WaitForTaskRunning();
			int GetThreadTimeout();

			//memory Read("AreaId", ...)
			virtual void Read(const char* id, UINT8 ReadData[], PUINT32 ReadDataSize);
			
		private:
			static DWORD WINAPI ThreadFunc(void* pArguments);
			void ThreadAreaRW(NodeDWordArray* area, UINT32 Data[], UINT16 Flag[]);
			void SetReadTimeStamp() { CAutoLock al(m_Lock); GetSystemTime(&m_ReadTime); }
			void SetWriteTimeStamp() { CAutoLock al(m_Lock); GetSystemTime(&m_WriteTime); }

		protected:
			OMC8000Protocol* GetProtocol() { return m_OMC8000Protocol.get(); }
			void ReadArea(const char* id, PUINT32 ReadData);

		public:
			typedef shared_ptr<OMC8000Task> OMC8000TaskPtr;

		private:
#pragma warning(push)
#pragma warning(disable:4251)
			OMC8000Protocol::OMC8000ProtocolPtr	m_OMC8000Protocol;
			CLock				m_Lock;
#pragma warning(pop)
			SYSTEMTIME			m_ReadTime;
			SYSTEMTIME			m_WriteTime;
			HANDLE				m_hThread;
			HANDLE				m_StopEvent;
			DWORD				m_threadID;
			UINT32				m_LoopTimeMS;
			bool				m_TaskIsRunning;
		};
	}
}

