#pragma once

#include "stdafx.h"
#include "OMC8000Task.h"

using namespace std;
using namespace Utilities::OMC8000;

namespace Utilities
{
	namespace OMC8000
	{
		class LIB_API OMC8000Lib
		{
		public:
			OMC8000Lib(void);
			~OMC8000Lib(void);

		public:
			//add TCP task
			OMC8000Task* AddTask(const char* Id, const char* IPaddress, UINT32 LoopTimeMS = 20);			
			//add UDP task
			OMC8000Task* AddTask(const char* Id, const char* IPaddress, UINT16 LocalPort, UINT16 RemotePort, UINT32 LoopTimeMS = 20);
			//add Serial task
			OMC8000Task* AddTask(const char* Id, const char* COMPort, SerialProtocol::BaudRate Baudrate, UINT32 LoopTimeMS = 20);
			
			OMC8000Task* GetTask(const char* Id);
			OMC8000Task* GetTaskByIndex(int Index);
			size_t GetTaskCount() { return m_Tasks.size(); }
			const char* GetTaskNameByIndex(int Index);

			OMC8000Task* Start(const char* TaskId, bool WaitForTaskRunning = false);
			OMC8000Task* Stop(const char* TaskId);
			void Clear(const char* TaskId);
			void Remove(const char* TaskId);

			void Start(bool WaitForTaskRunning = false);
			void Stop(bool WaitForTaskStopped = false);
			void Clear(void);

			//check all tasks are running
			bool TasksRunning(void);
			bool TasksStopped(void);

			//check single task
			bool TaskIsRunning(const char* TaskId);

			//check single task with exception
			void CheckTaskIsRunning(const char* Path);

			//node BIT Read("TaskId.NodeId.SubNodeId", &val);
			void Read(const char* Path, bool* Val, bool WaitForNewDataAvailable = false);
			//node BYTE Read("TaskId.NodeId.SubNodeId", &val);
			void Read(const char* Path, PUINT8 Val, bool WaitForNewDataAvailable = false);
			//node WORD Read("TaskId.NodeId.SubNodeId", &val);
			void Read(const char* Path, PUINT16 Val, bool WaitForNewDataAvailable = false);
			//node BIT, BYTE, WORD Read("TaskId.NodeId.SubNodeId", &val);
			void Read(const char* Path, UINT32_UT *Val, bool WaitForNewDataAvailable = false);

			//node BIT Write("TaskId.NodeId.SubNodeId", val);
			void Write(const char* Path, bool Val, bool WaitForWriteDataAcknowledge = false);
			//node BYTE Write("TaskId.NodeId.SubNodeId", val);
			void Write(const char* Path, UINT8 Val, bool WaitForWriteDataAcknowledge = false);
			//node WORD Write("TaskId.NodeId.SubNodeId", val);
			void Write(const char* Path, UINT16 Val, bool WaitForWriteDataAcknowledge = false);
			//node BIT, BYTE, WORD Write("TaskId.NodeId.SubNodeId", val);
			void Write(const char* Path, UINT32_UT Val, bool WaitForWriteDataAcknowledge = false);

			//OMC8000TaskDefault: memory Read("TaskId.AreaId", ...)
			void Read(const char* Path, UINT8 ReadData[], PUINT32 ReadDataSize);
			
			INode* GetNodeInterface(const char* id);
			INode* GetNodeInterfaceFromPath(const char* path);
			//SearchNodeInterface("TaskId", "NodeId.SubNodeId")
			list<INode*> SearchNodeInterface(const char* TaskId, const char* NodeId);

		private:
#pragma warning(push)
#pragma warning(disable:4251)
			map<string, OMC8000Task::OMC8000TaskPtr> m_Tasks;
#pragma warning(pop)
		};
	}
}