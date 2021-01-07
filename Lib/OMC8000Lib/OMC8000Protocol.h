#pragma once

#include "OMC8000Tcp.h"
#include "OMC8000Udp.h"
#include "OMC8000Serial.h"

using namespace Utilities::OMC8000;

namespace Utilities
{
	namespace OMC8000
	{
#pragma warning(push)
#pragma warning(disable:4275)

		enum OMC8000ProtocolType
		{
			OMC8000ProtocolTCP,
			OMC8000ProtocolUDP,
			OMC8000ProtocolSerial
		};

		class LIB_API OMC8000Protocol : public OMC8000Tcp, OMC8000Udp, OMC8000Serial
		{
		public:			
			OMC8000Protocol(const char* IPaddress) 
				: OMC8000Tcp(IPaddress), m_Type(OMC8000ProtocolTCP) {};
			OMC8000Protocol(const char* IPaddress, UINT16 PortLocal, UINT16 PortRemote)
				: OMC8000Udp(IPaddress, PortLocal, PortRemote), m_Type(OMC8000ProtocolUDP) {};
			OMC8000Protocol(const char* COMPort, SerialProtocol::BaudRate Baudrate, SerialProtocol::DataBits Databits = SerialProtocol::DB_8, SerialProtocol::Parity Parity = SerialProtocol::P_NONE, SerialProtocol::StopBits Stopbit = SerialProtocol::SB_ONE)
				: OMC8000Serial(COMPort, Baudrate, Databits, Parity, Stopbit), m_Type(OMC8000ProtocolSerial), m_SerialDevId(49) {};
			~OMC8000Protocol(void) {};

		public:
			
			void ReadVariable(const char* Variable, PUINT32 ReadData);
			void WriteVariable(const char* Variable, UINT32 WriteData);
			void WriteVariable(const char* Variable, FLOAT WriteData);

			bool IsInitialized(void);
			const char* GetDeviceId();
			int GetTimeout();
			void SetSerialDevId(UINT8 DevId) { m_SerialDevId = DevId; }
			OMC8000ProtocolType GetProtocolType() { return m_Type; }

		public:
			typedef shared_ptr<OMC8000Protocol> OMC8000ProtocolPtr;

		private:
			UINT8 m_SerialDevId;
			OMC8000ProtocolType m_Type;
		};
#pragma warning(pop)
	}
}


