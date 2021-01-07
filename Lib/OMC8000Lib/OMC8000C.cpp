#include "stdafx.h"
#include "ModBusC.h"
#include "ModBusLib.h"

using namespace std;
using namespace Utils::ModBus;

static ModBusException m_Exception = ModBusException();

int ModBusNode_AddNode(struct _SModBusNode* me, const char*id, enum EModBusNodeType type, unsigned int offset)
{
	if (Utils::ModBus::ModBusNodeInterface* p = reinterpret_cast<Utils::ModBus::ModBusNodeInterface*>(me->_h))
	{
		try
		{
			ModBusNodeType ntype = (ModBusNodeType)type;
			p->AddNode(id, ntype, offset);
		}
		catch(ModBusException &e)
		{
			m_Exception = e;
			return -1;
		}
		catch(...)
		{
			m_Exception = ModBusException("ModBusNode_AddNode() Unhandled Exception");
			return -2;
		}
	}

	return 0;
}

int ModBusNode_Read(struct _SModBusNode* me, PUINT16 val)
{
	if (Utils::ModBus::ModBusNodeInterface* p = reinterpret_cast<Utils::ModBus::ModBusNodeInterface*>(me->_h))
	{
		try
		{
			switch(p->Type())
			{
			case Utils::ModBus::ModBusNodeBit:
				{
					bool bval;
					p->Read(&bval);
					if(val) *val = (bval)? 1:0;
				}
				break;
			case Utils::ModBus::ModBusNodeByte:
				{
					UINT8 bval;
					p->Read(&bval);
					if(val) *val = bval;
				}
				break;
			case Utils::ModBus::ModBusNodeWord:
				p->Read(val);
				break;
			}
		}
		catch(ModBusException &e)
		{
			m_Exception = e;
			return -1;
		}
		catch(...)
		{
			m_Exception = ModBusException("ModBusNode_Read() Unhandled Exception");
			return -2;
		}
	}

	return 0;
}

int ModBusNode_Write(struct _SModBusNode* me, UINT16 val)
{
	if (Utils::ModBus::ModBusNodeInterface* p = reinterpret_cast<Utils::ModBus::ModBusNodeInterface*>(me->_h))
	{
		try
		{
			switch(p->Type())
			{
			case Utils::ModBus::ModBusNodeBit:
				{
					bool bval = (val!=0);
					p->Write(val);
				}
				break;
			case Utils::ModBus::ModBusNodeByte:
				{
					UINT8 bval = (UINT8)val;
					p->Write(val);
				}
				break;
			case Utils::ModBus::ModBusNodeWord:
				p->Write(val);
				break;
			}
		}
		catch(ModBusException &e)
		{
			m_Exception = e;
			return -1;
		}
		catch(...)
		{
			m_Exception = ModBusException("ModBusNode_Write() Unhandled Exception");
			return -2;
		}
	}

	return 0;
}

int ModBusNode_OnValueChanged(struct _SModBusNode* me, OnValueChanged_t EventFncPtr)
{
	if (Utils::ModBus::ModBusNodeInterface* p = reinterpret_cast<Utils::ModBus::ModBusNodeInterface*>(me->_h))
	{
		try
		{
			p->OnValueChanged += EventFncPtr;
		}
		catch(ModBusException &e)
		{
			m_Exception = e;
			return -1;
		}
		catch(...)
		{
			m_Exception = ModBusException("Unhandled Exception");
			return -2;
		}
	}

	return 0;
}

SModBusNodePtr ModBusNode_GetNode(struct _SModBusNode* me, const char*id)
{
	if (Utils::ModBus::ModBusNodeInterface* p = reinterpret_cast<Utils::ModBus::ModBusNodeInterface*>(me->_h))
	{
		try
		{
			ModBusNodeInterface* node = p->GetNodeInterface(id);
			SModBusNodePtr pnode = new SModBusNode();
			pnode->_h = reinterpret_cast<HANDLE>(node);
			pnode->AddNode = ModBusNode_AddNode;
			pnode->GetNode = ModBusNode_GetNode;
			pnode->OnValueChanged = ModBusNode_OnValueChanged;
			pnode->Write = ModBusNode_Write;
			pnode->Read = ModBusNode_Read;

			return pnode;
		}
		catch(ModBusException &e)
		{
			m_Exception = e;
			return NULL;
		}
		catch(...)
		{
			m_Exception = ModBusException("ModBusNode_GetNode() Unhandled Exception");
			return NULL;
		}
	}

	return NULL;
}

int ModBusTask_AddRegisterArea(struct _SModBusTask* me, const char* id, UINT16 StartingAddress, UINT16 RequestedWordCount)
{
	if (Utils::ModBus::ModBusTask* p = reinterpret_cast<Utils::ModBus::ModBusTask*>(me->_h))
	{
		try
		{
			p->AddRegisterArea(id, StartingAddress, RequestedWordCount);
		}
		catch(ModBusException &e)
		{
			m_Exception = e;
			return -1;
		}
		catch(...)
		{
			m_Exception = ModBusException("ModBusTask_AddRegisterArea() Unhandled Exception");
			return -2;
		}
	}

	return 0;
}

int ModBusTask_AddCoilArea(struct _SModBusTask* me, const char*id, UINT16 StartingAddress, UINT16 RequestedBitCount)
{
	if (Utils::ModBus::ModBusTask* p = reinterpret_cast<Utils::ModBus::ModBusTask*>(me->_h))
	{
		try
		{
			p->AddCoilArea(id, StartingAddress, RequestedBitCount);
		}
		catch(ModBusException &e)
		{
			m_Exception = e;
			return -1;
		}
		catch(...)
		{
			m_Exception = ModBusException("ModBusTask_AddCoilArea() Unhandled Exception");
			return -2;
		}
	}

	return 0;
}

SModBusNodePtr ModBusTask_GetNode(struct _SModBusTask* me, const char*id)
{
	if (ModBusTask* p = reinterpret_cast<ModBusTask*>(me->_h))
	{
		try
		{
			ModBusNodeInterface* node = p->GetNodeInterface(id);
			SModBusNodePtr pnode = new SModBusNode();
			pnode->_h = reinterpret_cast<HANDLE>(node);
			pnode->AddNode = ModBusNode_AddNode;
			pnode->GetNode = ModBusNode_GetNode;
			pnode->OnValueChanged = ModBusNode_OnValueChanged;
			pnode->Write= ModBusNode_Write;
			pnode->Read = ModBusNode_Read;

			return pnode;
		}
		catch(ModBusException &e)
		{
			m_Exception = e;
			return NULL;
		}
		catch(...)
		{
			m_Exception = ModBusException("ModBusTask_GetNode() Unhandled Exception");
			return NULL;
		}
	}

	return NULL;
}

int ModBus_AddTask(struct _SModBus* me, const char* id, const char* IPaddress)
{
	if (ModBusLib* p = reinterpret_cast<ModBusLib*>(me->_h))
	{
		try
		{
			p->AddTask(id, IPaddress);
		}
		catch(ModBusException &e)
		{
			m_Exception = e;
			return -1;
		}
		catch(...)
		{
			m_Exception = ModBusException("ModBus_AddTask() Unhandled Exception");
			return -2;
		}
	}

	return 0;
}

int ModBus_AddTaskSpecific(struct _SModBus* me, const char* id, const char* IPaddress, UINT16 LoopTimeMS, EModBusTaskType TaskType)
{
	if (ModBusLib* p = reinterpret_cast<ModBusLib*>(me->_h))
	{
		try
		{
			p->AddTask(id, IPaddress, LoopTimeMS, (ModBusTaskType)TaskType);
		}
		catch(ModBusException &e)
		{
			m_Exception = e;
			return -1;
		}
		catch(...)
		{
			m_Exception = ModBusException("ModBus_AddTaskSpecific() Unhandled Exception");
			return -2;
		}
	}

	return 0;
}

int ModBus_Start(struct _SModBus* me, const char* id)
{
	if (ModBusLib* p = reinterpret_cast<ModBusLib*>(me->_h))
	{
		try
		{
			p->Start(id);
		}
		catch(ModBusException &e)
		{
			m_Exception = e;
			return -1;
		}
		catch(...)
		{
			m_Exception = ModBusException("ModBus_Start() Unhandled Exception");
			return -2;
		}
	}

	return 0;
}

int ModBus_Stop(struct _SModBus* me, const char* id)
{
	if (ModBusLib* p = reinterpret_cast<ModBusLib*>(me->_h))
	{
		try
		{
			p->Stop(id);
		}
		catch(ModBusException &e)
		{
			m_Exception = e;
			return -1;
		}
		catch(...)
		{
			m_Exception = ModBusException("ModBus_Stop() Unhandled Exception");
			return -2;
		}
	}

	return 0;
}

int ModBus_Clear(struct _SModBus* me, const char* id)
{
	if (ModBusLib* p = reinterpret_cast<ModBusLib*>(me->_h))
	{
		try
		{
			p->Clear(id);
		}
		catch(ModBusException &e)
		{
			m_Exception = e;
			return -1;
		}
		catch(...)
		{
			m_Exception = ModBusException("ModBus_Clear() Unhandled Exception");
			return -2;
		}
	}

	return 0;
}

int ModBus_Read(struct _SModBusNode* me, const char* path, PUINT16 val)
{
	if (ModBusLib* p = reinterpret_cast<ModBusLib*>(me->_h))
	{
		try
		{
			ModBusNodeInterface* node = p->GetNodeInterfaceFromPath(path);

			switch(node->Type())
			{
			case Utils::ModBus::ModBusNodeBit:
				{
					bool bval;
					node->Read(&bval);
					if(val) *val = (bval)? 1:0;
				}
				break;
			case Utils::ModBus::ModBusNodeByte:
				{
					UINT8 bval;
					node->Read(&bval);
					if(val) *val = bval;
				}
				break;
			case Utils::ModBus::ModBusNodeWord:
				node->Read(val);
				break;
			}
		}
		catch(ModBusException &e)
		{
			m_Exception = e;
			return -1;
		}
		catch(...)
		{
			m_Exception = ModBusException("ModBus_Read() Unhandled Exception");
			return -2;
		}
	}

	return 0;
}

int ModBus_Write(struct _SModBusNode* me, const char* path, UINT16 val)
{
	if (ModBusLib* p = reinterpret_cast<ModBusLib*>(me->_h))
	{
		try
		{
			ModBusNodeInterface* node = p->GetNodeInterfaceFromPath(path);

			switch(node->Type())
			{
			case Utils::ModBus::ModBusNodeBit:
				{
					bool bval = (val!=0);
					node->Write(val);
				}
				break;
			case Utils::ModBus::ModBusNodeByte:
				{
					UINT8 bval = (UINT8)val;
					node->Write(val);
				}
				break;
			case Utils::ModBus::ModBusNodeWord:
				node->Write(val);
				break;
			}
		}
		catch(ModBusException &e)
		{
			m_Exception = e;
			return -1;
		}
		catch(...)
		{
			m_Exception = ModBusException("ModBus_Write() Unhandled Exception");
			return -2;
		}
	}

	return 0;
}

int ModBus_ReadMsg(struct _SModBusNode* me, const char* Path, UINT8 ReadData[], PUINT32 ReadDataSize)
{
	if (ModBusLib* p = reinterpret_cast<ModBusLib*>(me->_h))
	{
		try
		{
			p->Read(Path, ReadData, ReadDataSize);
		}
		catch(ModBusException &e)
		{
			m_Exception = e;
			return -1;
		}
		catch(...)
		{
			m_Exception = ModBusException("ModBus_ReadMsg() Unhandled Exception");
			return -2;
		}
	}

	return 0;
}

int ModBus_ReadMemory(struct _SModBusNode* me, const char* Path, UINT32 Address, UINT32 ReadRequest, UINT8 ReadData[], PUINT32 ReadDataSize)
{
	if (ModBusLib* p = reinterpret_cast<ModBusLib*>(me->_h))
	{
		try
		{
			p->Read(Path, Address, ReadRequest, ReadData, ReadDataSize);
		}
		catch(ModBusException &e)
		{
			m_Exception = e;
			return -1;
		}
		catch(...)
		{
			m_Exception = ModBusException("ModBus_ReadMemory() Unhandled Exception");
			return -2;
		}
	}

	return 0;
}


int ModBus_WriteMemory(struct _SModBusNode* me, const char* Path, UINT32 Address, UINT8 WriteData[], UINT32 WriteDataSize)
{
	if (ModBusLib* p = reinterpret_cast<ModBusLib*>(me->_h))
	{
		try
		{
			p->Write(Path, Address, WriteData, WriteDataSize);
		}
		catch(ModBusException &e)
		{
			m_Exception = e;
			return -1;
		}
		catch(...)
		{
			m_Exception = ModBusException("ModBus_WriteMemory() Unhandled Exception");
			return -2;
		}
	}

	return 0;
}

SModBusTaskPtr ModBus_GetTask(struct _SModBus* me, const char* id)
{
	if (ModBusLib* p = reinterpret_cast<ModBusLib*>(me->_h))
	{
		try
		{
			ModBusTask* task = p->GetTask(id);

			SModBusTaskPtr ptask = new SModBusTask();
			ptask->_h = reinterpret_cast<HANDLE>(task);
			ptask->AddCoilArea = ModBusTask_AddCoilArea;
			ptask->AddRegisterArea = ModBusTask_AddRegisterArea;
			ptask->GetNode = ModBusTask_GetNode;

			return ptask;
		}
		catch(ModBusException &e)
		{
			m_Exception = e;
			return NULL;
		}
		catch(...)
		{
			m_Exception = ModBusException("ModBus_GetTask() Unhandled Exception");
			return NULL;
		}
	}

	return NULL;
}

void modbusnode_delete(SModBusNodePtr* pModBusNode)
{
	SModBusNodePtr pnode = *pModBusNode;
	if(pnode!=NULL)
	{
		delete pnode;
		*pModBusNode = NULL;
	}
}

void modbustask_delete(SModBusTaskPtr* pModBusTask)
{
	SModBusTaskPtr ptask = *pModBusTask;
	if(ptask!=NULL)
	{
		delete ptask;
		*pModBusTask = NULL;
	}
}

void modbus_new(SModBusPtr* pModBus)
{
	if (ModBusLib* p = new ModBusLib())
	{
		SModBusPtr pmodbus = new SModBus();
		pmodbus->_h = reinterpret_cast<HANDLE>(p);
		pmodbus->AddTask = ModBus_AddTask;
		pmodbus->AddTaskSpecific = ModBus_AddTaskSpecific;
		pmodbus->GetTask = ModBus_GetTask;
		pmodbus->Start = ModBus_Start;
		pmodbus->Stop = ModBus_Stop;
		pmodbus->Clear = ModBus_Clear;
		pmodbus->Read = ModBus_Read;
		pmodbus->Write = ModBus_Write;
		pmodbus->ReadMsg = ModBus_ReadMsg;
		pmodbus->ReadMemory = ModBus_ReadMemory;
		pmodbus->WriteMemory = ModBus_WriteMemory;

		*pModBus = pmodbus;
	}
}

void modbus_delete(SModBusPtr* pModBus)
{
	SModBusPtr pmodbus = *pModBus;
	if(pmodbus!=NULL)
	{
		if (ModBusLib* p = reinterpret_cast<ModBusLib*>(pmodbus->_h))
		{
			delete p;
		}
		delete pmodbus;
		*pModBus = NULL;
	}
}

void ModBusGetLastError(char* msg, unsigned int msgSize)
{
	if(!m_Exception.empty())
	{
		sprintf_s(msg, msgSize, "%s; id:%s; func:%s; file:%s; line:%d", m_Exception.what(), m_Exception.id(), m_Exception.fnc(), m_Exception.file(), m_Exception.line());
	}
	else
	{
		sprintf_s(msg, msgSize, "");
	}
}