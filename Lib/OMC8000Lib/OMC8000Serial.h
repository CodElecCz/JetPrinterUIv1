#pragma once

#include "stdafx.h"
#include "SerialProtocol.h"

using namespace Utilities;

namespace Utilities
{
	namespace OMC8000
	{
		class OMC8000Serial : public SerialProtocol
		{
		public:
			OMC8000Serial(void) : SerialProtocol() {};
			OMC8000Serial(const char* COMPort, SerialProtocol::BaudRate Baudrate = BR_9600, SerialProtocol::DataBits Databits = DB_8, SerialProtocol::Parity Parity = P_NONE, SerialProtocol::StopBits Stopbit = SB_ONE)
				: SerialProtocol(COMPort, Baudrate, Databits, Parity, Stopbit) {};
			~OMC8000Serial(void) {};

		public:
			void Query(UINT8 WriteData[], UINT16 WriteCount, UINT8 ReadData[], PUINT32 ReadCount, UINT32 ReadCountRequest, UINT8 DevId = 1);
			bool IsInitialized(void) { return SerialProtocol::IsInitialized(); };
			const char* GetCOMPort() { return SerialProtocol::GetCOMPort(); };
			int GetTimeout() { return SerialProtocol::GetTimeout(); };

		private:
			CLock m_OMC8000SerialLock;
		};
	}
}

