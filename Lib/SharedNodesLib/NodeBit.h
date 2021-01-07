#pragma once

#include "stdafx.h"
#include "Def.h"

using namespace std;
using namespace Utilities;

namespace Utilities
{
	namespace Node
	{
		class LIB_API NodeBitShared
		{
		public:
			NodeBitShared(bool& ReadVar, bool& WriteVar, UINT16& ReadFlag, UINT16& WriteFlag, CLock& ReadLock, CLock& WriteLock, SYSTEMTIME& ReadTimeStamp, SYSTEMTIME& WriteTimeStamp)
				: m_rValue(ReadVar), m_wValue(WriteVar), m_rFlag(ReadFlag), m_wFlag(WriteFlag), m_ReadLock(ReadLock), m_WriteLock(WriteLock), m_ReadTime(ReadTimeStamp), m_WriteTime(WriteTimeStamp)
			{};

			virtual ~NodeBitShared(void) {};

		protected:
			//lock
			CLock& m_ReadLock;
			CLock& m_WriteLock;

			//value
			bool& m_rValue;
			bool& m_wValue;
			
			//flag
			UINT16& m_rFlag;		
			UINT16& m_wFlag;		

			//timestamp
			SYSTEMTIME& m_ReadTime;
			SYSTEMTIME& m_WriteTime;
		};

		class LIB_API NodeBit : public INode, private NodeBitShared
		{
		public:
			NodeBit(string Id, NodeType Type, NodeAccess Access, bool& ReadVar, bool& WriteVar, UINT16& ReadFlag, UINT16& WriteFlag, CLock& ReadLock, CLock& WriteLock, SYSTEMTIME& ReadTimeStamp, SYSTEMTIME& WriteTimeStamp, UINT32 DataOffset = 0)
				: INode(Id, Type, Access, DataOffset), NodeBitShared(ReadVar, WriteVar, ReadFlag, WriteFlag, ReadLock, WriteLock, ReadTimeStamp, WriteTimeStamp), m_rValueInit(true)
			{};

			~NodeBit(void)
			{};
		
			void Read(bool* val) { CAutoLock al(m_ReadLock); if(val) *val = m_rValue; m_rFlag = 0; }

			void Write(bool val) { CAutoLock al(m_WriteLock); m_wValue = val; m_wFlag++; }

			void Write(bool* val) { CAutoLock al(m_WriteLock); if(val) *val = m_wValue; }

			bool ReadDataAvailable() { CAutoLock al(m_ReadLock); return (m_rFlag>0); };
			bool WriteDataAcknowledge() { CAutoLock al(m_WriteLock); return (m_wFlag==0); };
			
			//nodes
			void Refresh(void);

			//timestamp
			SYSTEMTIME ReadTimeStamp() { CAutoLock al(m_ReadLock); return m_ReadTime; }
			SYSTEMTIME WriteTimeStamp() { CAutoLock al(m_WriteLock); return m_WriteTime; }

		public:
			typedef std::shared_ptr<NodeBit> NodeBitPtr;
			
		private:
			//OnNodeValueChanged
			bool m_rValueLast;
			bool m_rValueInit;
		};
	}
}
