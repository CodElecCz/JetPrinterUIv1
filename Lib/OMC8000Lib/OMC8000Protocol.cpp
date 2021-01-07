#include "stdafx.h"
#include "OMC8000Protocol.h"

namespace Utilities
{
	namespace OMC8000
	{
		void FromByte(UINT8 c, bool b[], int bitSizeLimit)
		{
			int max = (bitSizeLimit<8)? bitSizeLimit:8;
			for (int i=0; i < max; ++i)
				b[i] = (c & (1<<i)) != 0;
		}

		void ToByte(bool b[], PUINT8 c, int bitSizeLimit)
		{
			int max = (bitSizeLimit<8)? bitSizeLimit:8;
			UINT8 res = 0;
			for (int i=0; i < max; ++i)
				res += b[i] << i;

			if(c) *c = res;
		}

		bool OMC8000Protocol::IsInitialized(void)
		{
			switch (m_Type)
			{
			case OMC8000ProtocolTCP:
				return OMC8000Tcp::IsInitialized();
			case OMC8000ProtocolUDP:
				return OMC8000Udp::IsInitialized();
			case OMC8000ProtocolSerial:
				return OMC8000Serial::IsInitialized();
			default:
				return false;
			}
		}
		
		const char* OMC8000Protocol::GetDeviceId() 
		{ 
			switch (m_Type)
			{
			case OMC8000ProtocolTCP:
				return OMC8000Tcp::GetIPaddress();
			case OMC8000ProtocolUDP:
				return OMC8000Udp::GetIPaddress();
			case OMC8000ProtocolSerial:
				return OMC8000Serial::GetCOMPort();
			default:
				return "";
			}
		}
		
		int OMC8000Protocol::GetTimeout()
		{ 
			switch (m_Type)
			{
			case OMC8000ProtocolTCP:
				return OMC8000Tcp::GetTimeout();
			case OMC8000ProtocolUDP:
				return OMC8000Udp::GetTimeout();
			case OMC8000ProtocolSerial:
				return OMC8000Serial::GetTimeout();
			default:
				return -1;
			}
		}

		void OMC8000Protocol::ReadVariable(const char* Variable, PUINT32 ReadData)
		{
			UINT8 wdata[64];
			int i;
			wdata[0] = '0';
			wdata[1] = '*';
			for (i = 0; i < strnlen_s(Variable, 32); i++)
			{
				wdata[i + 2] = Variable[i];
			}
			UINT16 wdataSize = i + 2;

			int rdataSizeReq = 32;
			vector<UINT8> rdata(rdataSizeReq);
			unsigned int rdataSize = rdataSizeReq;

			switch(m_Type)
			{
			case OMC8000ProtocolTCP:
				OMC8000Tcp::Query(wdata, wdataSize, rdata.data(), &rdataSize, rdataSizeReq);
				break;
			case OMC8000ProtocolUDP:
				OMC8000Udp::Query(wdata, wdataSize, rdata.data(), &rdataSize, rdataSizeReq);
				break;
			case OMC8000ProtocolSerial:
				OMC8000Serial::Query(wdata, wdataSize, rdata.data(), &rdataSize, rdataSizeReq, m_SerialDevId);
				break;
			}

			UINT32_UT val;
			char* endPtr = NULL;

			UINT32 uval = strtoul((const char*)rdata.data(), &endPtr, 10);
			if (endPtr != NULL && endPtr[0] == '\0')
			{
				val.dword = uval;
			}
			else
			{
				float fval = (float)strtod((const char*)rdata.data(), NULL);
				val.real32 = fval;
			}
			
			if(ReadData) 
				*ReadData = val.dword;
		}

		void OMC8000Protocol::WriteVariable(const char* Variable, UINT32 WriteData)
		{
			UINT8 wdata[64];
			int i, j;
			wdata[0] = '0';
			wdata[1] = '{';

			char snumber[32];
			sprintf_s(snumber, sizeof(snumber), "%d", WriteData);
			for (i = 0; i < strnlen_s(snumber, 32); i++)
			{
				wdata[i + 2] = snumber[i];
			}
			i += 2;
			wdata[i++] = ',';

			for (j = 0; j < strnlen_s(Variable, 32); j++)
			{
				wdata[i + j] = Variable[j];
			}
			UINT16 wdataSize = i + j;

			int rdataSizeReq = 32;
			vector<UINT8> rdata(rdataSizeReq);
			unsigned int rdataSize = rdataSizeReq;

			switch(m_Type)
			{
			case OMC8000ProtocolTCP:
				OMC8000Tcp::Query(wdata, wdataSize, rdata.data(), &rdataSize, rdataSizeReq);
				break;
			case OMC8000ProtocolUDP:
				OMC8000Udp::Query(wdata, wdataSize, rdata.data(), &rdataSize, rdataSizeReq);
				break;
			case OMC8000ProtocolSerial:
				OMC8000Serial::Query(wdata, wdataSize, rdata.data(), &rdataSize, rdataSizeReq, m_SerialDevId);
				break;
			}

			if(rdataSize>0 && rdata[0]!='!')
			{
				char msg[256];
				sprintf_s(msg, sizeof(msg), "OMC8000 wrong reponse, expected '!'");
				throw Exc(GetDeviceId(), msg);
			}
		}

		void OMC8000Protocol::WriteVariable(const char* Variable, FLOAT WriteData)
		{
			UINT8 wdata[64];
			int i, j;
			wdata[0] = '0';
			wdata[1] = '}';

			for (i = 0; i < strnlen_s(Variable, 32); i++)
			{
				wdata[i + 2] = Variable[i];
			}
			i += 2;
			wdata[i++] = ',';

			char snumber[32];
			sprintf_s(snumber, sizeof(snumber), "%0.02f", WriteData);
			for (j = 0; j < strnlen_s(snumber, 32); j++)
			{
				wdata[i + j] = snumber[j];
			}
			UINT16 wdataSize = i + j;

			int rdataSizeReq = 32;
			vector<UINT8> rdata(rdataSizeReq);
			unsigned int rdataSize = rdataSizeReq;

			switch (m_Type)
			{
			case OMC8000ProtocolTCP:
				OMC8000Tcp::Query(wdata, wdataSize, rdata.data(), &rdataSize, rdataSizeReq);
				break;
			case OMC8000ProtocolSerial:
				OMC8000Serial::Query(wdata, wdataSize, rdata.data(), &rdataSize, rdataSizeReq, m_SerialDevId);
				break;
			}

			if (rdataSize>0 && rdata[0] != '!')
			{
				char msg[256];
				sprintf_s(msg, sizeof(msg), "OMC8000 wrong reponse, expected '!'");
				throw Exc(GetDeviceId(), msg);
			}
		}
	}
}