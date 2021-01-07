#pragma once

#include "Serial.h"
#include "Lock.h"

using namespace Utils;

namespace Utils
{
	namespace JetPrinter
	{
		class JetPrinterSerial : public Serial
		{
		public:
			JetPrinterSerial(void) : Serial() {};
			JetPrinterSerial(const char* COMPort, Serial::BaudRate Baudrate = BR_9600, Serial::DataBits Databits = DB_8, Serial::Parity Parity = P_NONE, Serial::StopBits Stopbit = SB_ONE)
				: Serial(COMPort, Baudrate, Databits, Parity, Stopbit) {};
			~JetPrinterSerial(void) {};

		public:
			void Query(UINT8 WriteData[], UINT16 WriteCount, UINT8 ReadData[], PUINT32 ReadCount);
			void Write(UINT8 WriteData[], UINT16 WriteCount);
			bool IsInitialized(void) { return Serial::IsInitialized(); };
			const char* GetCOMPort() { return Serial::GetCOMPort(); };
			int GetTimeout() { return Serial::GetTimeout(); };

		private:
			CLock m_JetPrinterSerialLock;
		};
	}
}

