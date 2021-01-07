#include "stdafx.h"
#include "OMC8000Task.h"

namespace Utilities
{
	namespace OMC8000
	{
		OMC8000Task::OMC8000Task(string Id) : INode(Id, NodeType_Task)
		{
			m_hThread = 0;
			m_threadID = 0;
			m_StopEvent = 0;
			m_TaskIsRunning = false;
			memset(&m_ReadTime, 0, sizeof(SYSTEMTIME));
			memset(&m_WriteTime, 0, sizeof(SYSTEMTIME));
		}

		OMC8000Task::~OMC8000Task(void)
		{
			Clear();
		}

		void OMC8000Task::Create(const char* IPaddress, UINT32 LoopTimeMS)
		{
			Clear();
			
			m_LoopTimeMS = LoopTimeMS;

			//TCP
			m_OMC8000Protocol = OMC8000Protocol::OMC8000ProtocolPtr(new OMC8000Protocol(IPaddress));
		}

		void OMC8000Task::Create(const char* IPaddress, UINT32 LoopTimeMS, UINT16 LocalPort, UINT16 RemotePort)
		{
			Clear();

			m_LoopTimeMS = LoopTimeMS;
	
			//UDP
			m_OMC8000Protocol = OMC8000Protocol::OMC8000ProtocolPtr(new OMC8000Protocol(IPaddress, LocalPort, RemotePort));
		}

		void OMC8000Task::Create(const char* COMport, UINT32 LoopTimeMS, SerialProtocol::BaudRate Baudrate)
		{
			Clear();
			
			m_LoopTimeMS = LoopTimeMS;
			m_OMC8000Protocol = OMC8000Protocol::OMC8000ProtocolPtr(new OMC8000Protocol(COMport, Baudrate));
		}

		NodeDWordArray* OMC8000Task::AddArea(const char* Id, const char* Variable, NodeAccess Access)
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

			NodeDWordArray::NodeDWordArrayPtr area(new NodeDWordArray(Id, NodeType_DWordArray, Access, 0, 1));
			area->SetVarStr(Variable);
			m_Nodes[id] = area;

			return area.get();
		}

		void OMC8000Task::Start(bool WaitForTaskRunning)
		{
			if(m_Nodes.size()==0)
				throw Exc(this->Id(), "No area was defined");

			if(m_hThread == 0)
			{
				m_StopEvent	= CreateEvent( NULL,   // No security attributes
					TRUE,  // Manual-reset event
					FALSE,   // Initial state is not signaled
					NULL);  // Object not named

				m_hThread = CreateThread(NULL, 0, &OMC8000Task::ThreadFunc, this, 0, &m_threadID);

				if(WaitForTaskRunning)
				{
					this->WaitForTaskRunning();
				}
			}
		}

		void OMC8000Task::Stop()
		{
			if(m_StopEvent)
			{
				SetEvent( m_StopEvent);

				int timeout = GetThreadTimeout();

				if(WaitForSingleObject(m_hThread, timeout))
				{
					//timeout
					if ( m_TaskIsRunning )
					{
						TerminateThread(m_hThread, 0);
						m_TaskIsRunning = false;
					}
				}
				m_threadID = 0;
				CloseHandle(m_StopEvent);
				m_StopEvent = 0;
				CloseHandle(m_hThread);
				m_hThread = 0;
			}
		}

		int OMC8000Task::GetThreadTimeout()
		{
			int timeout = 0;

			if(m_Nodes.size())
			{
				size_t nbOfAreas = m_Nodes.size();
				timeout = (int)nbOfAreas * (this->m_OMC8000Protocol.get()->GetTimeout() + 200);
			}

			return timeout;
		}

		void OMC8000Task::Clear()
		{
			Stop(); 

			m_Nodes.clear();
		}

		list<INode*> OMC8000Task::SearchNodeInterface(const char* Id)
		{
			string id(Id);
			list<INode*> nodelist;

			map<string, INode::INodePtr>::iterator it = m_Nodes.find(id);
			if(it!=m_Nodes.end())
			{
				nodelist.push_back(it->second.get());
			}
			else
			{
				it = m_Nodes.begin();
				while(it!=m_Nodes.end())
				{
					if(it->second->NodesCount()>0)
					{
						list<INode*> sublist = it->second->SearchNodeInterface(Id);
						nodelist.insert(nodelist.end(), sublist.begin(), sublist.end());
					}
					it++;
				}
			}
		
			return nodelist;
		}

		void OMC8000Task::ReadArea(const char* Id, PUINT32 ReadData)
		{
			UINT32 readData[1];
			UINT32 dataSize = 1;
			GetNodeInterface(Id)->Read(readData, &dataSize);

			if (ReadData) *ReadData = readData[0];
		}

		void OMC8000Task::Read(const char* id, UINT8 ReadData[], PUINT32 ReadDataSize)
		{
			UINT32 ReadDataSizeAlloc = *ReadDataSize;
			*ReadDataSize = 0;

			NodeDWordArray* area = dynamic_cast<NodeDWordArray*>(GetNodeInterface(id));

			UINT32 data[1];
			UINT32 dataSize = 1;
			area->Read(data, &dataSize);
			dataSize = sizeof(data);

			memcpy_s(ReadData, ReadDataSizeAlloc, &data, dataSize*2);
			*ReadDataSize = dataSize*2;
		}

		void OMC8000Task::WaitForTaskRunning()
		{
			int timeout = GetThreadTimeout();
			clock_t t = clock();
			UINT32 wait = ((m_LoopTimeMS/2)>2)? (m_LoopTimeMS/2):2;
			while(!m_TaskIsRunning)
			{
				UINT32 elapsed = (UINT32)(clock() - t);
				if(elapsed > (UINT32)timeout)
					throw Exc(Id(), "WaitForTaskRunning() error: timeout");

				Sleep(wait);
			}
		}

		void OMC8000Task::ThreadAreaRW(NodeDWordArray* area, UINT32 Data[], UINT16 Flag[])
		{
			if(area->Access()&NodeAccess_Write)
			{																		
				//write				
				UINT32 dataSize = 1;
				area->GetWriteArea(Data, Flag, &dataSize);
				
				if(Flag[0])
				{				
					this->m_OMC8000Protocol->WriteVariable(area->GetVarStr().data(), Data[0]);
					
					//set timestamp
					SetWriteTimeStamp();
					
					//write data acknoledge
					area->SetWriteAreaAcknoledge(0, 1);
				}								
			}

			if(area->Access()&NodeAccess_Read)
			{									
				//read
				UINT32 dataSize = area->Count();
				this->m_OMC8000Protocol->ReadVariable(area->GetVarStr().data(), &Data[0]);
				
				//set timestamp
				SetReadTimeStamp();
				
				//copy read data
				area->SetReadArea(Data, 1);	

				//init write area
				if(!this->m_TaskIsRunning)
					area->SetWriteArea(Data, 1);
			}
		}

		DWORD WINAPI OMC8000Task::ThreadFunc(void* pArguments)
		{
			OMC8000Task*	me = (OMC8000Task*)pArguments;
			UINT32 wait = -1;
			clock_t t;
			UINT32 WAIT = me->m_LoopTimeMS; //ms default 20ms

			UINT32 data[1];
			UINT16 flag[1];

			do
			{
				t = clock();

				map<string, INode::INodePtr>::iterator it = me->m_Nodes.begin();
				while (it != me->m_Nodes.end())
				{
					NodeDWordArray* area = dynamic_cast<NodeDWordArray*>(it->second.get());
					
					try
					{
						switch(it->second->Type())
						{
						case NodeType_DWordArray: //UINT32							
							me->ThreadAreaRW(area, data, flag);
							break;
						}

						area->ClearExceptionForAllNodes();
						me->ClearException();
					}
					catch(INodeException &e)
					{
						char msg[512];
						e.str(msg, sizeof(msg));
						printf("%s\n", msg);
						area->SetExceptionForAllNodes(e);
						me->SetException(e);
					}
					it++;
				}
				
				if(!me->m_TaskIsRunning)
					me->m_TaskIsRunning = true;

				//calculate next wait
				UINT32 elapsed = (UINT32)(clock() - t);
				if(WAIT > elapsed)
					wait = WAIT - elapsed;
				else
				{
#ifdef _DEBUG
					if(elapsed>(WAIT*1.5))
						printf("Task timeout: %d [ms]; default: %d [ms]\n", elapsed, WAIT);
#endif
					wait = 1;
				}
			} while(WaitForSingleObject( me->m_StopEvent, wait) != WAIT_OBJECT_0);

			me->m_TaskIsRunning = false;

			return 0;
		}
	}
}
