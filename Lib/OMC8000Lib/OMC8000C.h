#ifndef MODBUSC_H
#define MODBUSC_H

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
#include <windows.h>

#ifdef __cplusplus
extern "C" {
#endif

enum EModBusNodeType
{
	ModBusNodeBit = 0,
	ModBusNodeByte,
	ModBusNodeWord
};

enum EModBusTaskType
{
	ModBusTaskDefault = 0,
	ModBusTaskTurckRfid
};

typedef void (*OnValueChanged_t)(const char*, UINT16);

typedef struct _SModBusNode
{
	int (*AddNode)(struct _SModBusNode* me, const char*id, enum EModBusNodeType type, unsigned int offset);
	struct _SModBusNode* (*GetNode)(struct _SModBusNode* me, const char*id);
 
	int (*Read)(struct _SModBusNode* me, PUINT16 val); 
	int (*Write)(struct _SModBusNode* me, UINT16 val); 

	int (*OnValueChanged)(struct _SModBusNode* me, OnValueChanged_t EventFncPtr);
	
	HANDLE _h;
}SModBusNode, *SModBusNodePtr;

__declspec(dllexport) void modbusnode_delete(SModBusNodePtr* pModBusNode);

typedef struct _SModBusTask
{
	int (*AddRegisterArea)(struct _SModBusTask* me, const char* id, UINT16 StartingAddress, UINT16 RequestedWordCount);
	int (*AddCoilArea)(struct _SModBusTask* me, const char*id, UINT16 StartingAddress, UINT16 RequestedBitCount);
	SModBusNodePtr (*GetNode)(struct _SModBusTask* me, const char*id);

	HANDLE _h;
}SModBusTask, *SModBusTaskPtr;

__declspec(dllexport) void modbustask_delete(SModBusTaskPtr* pModBusTask);

typedef struct _SModBus
{
	int (*AddTask)(struct _SModBus* me, const char* id, const char* IPaddress);
	int (*AddTaskSpecific)(struct _SModBus* me, const char* id, const char* IPaddress, UINT16 LoopTime, enum EModBusTaskType TaskType);
	SModBusTaskPtr (*GetTask)(struct _SModBus* me, const char* id);

	int (*Start)(struct _SModBus* me, const char* id);
	int (*Stop)(struct _SModBus* me, const char* id);
	int (*Clear)(struct _SModBus* me, const char* id);

	int (*Read)(struct _SModBusNode* me, const char* path, PUINT16 val); 
	int (*Write)(struct _SModBusNode* me, const char* path, UINT16 val);
	
	int (*ReadMsg)(struct _SModBusNode* me, const char* path, UINT8 ReadData[], PUINT32 ReadDataSize); 

	int (*ReadMemory)(struct _SModBusNode* me, const char* path, UINT32 Address, UINT32 ReadRequest, UINT8 ReadData[], PUINT32 ReadDataSize); 
	int (*WriteMemory)(struct _SModBusNode* me, const char* path, UINT32 Address, UINT8 WriteData[], UINT32 WriteDataSize);

	HANDLE _h;
}SModBus, *SModBusPtr;

__declspec(dllexport) void modbus_new(SModBusPtr* pModBus);
__declspec(dllexport) void modbus_delete(SModBusPtr* pModBus);
__declspec(dllexport) void ModBusGetLastError(char* msg, unsigned int msgSize);

#ifdef __cplusplus 
}
#endif

#endif