#include "stdafx.h"
#include "JetPrinterSerial.h"
#include "JetPrinterException.h"

namespace Utils
{
	namespace JetPrinter
	{
#ifdef JETPRINETRSERIAL_LOG
		void Log(const char* COMPort, const char* id, UINT8 data[], UINT32 dataSize, SYSTEMTIME t)
		{
			vector<char> sbuffer(dataSize*3 + 64);
			char* pend = sbuffer.data();

			pend += sprintf(pend, "[%02d:%02d:%02d.%03d] %s[%s] 0x%x ", (int)t.wHour, (int)t.wMinute, (int)t.wSecond, (int)t.wMilliseconds, id, IPaddress, GetCurrentThreadId());
			//header
			pend += sprintf(pend, "H[");
			for (int i = 0; i < 7; i++)
				pend += sprintf(pend, "%02x ", (unsigned int)data[i]);
			pend--;
			pend += sprintf(pend, "] ");
			//data
			pend += sprintf(pend, "D[");
			for (unsigned int i = 7; i < dataSize; i++)
				pend += sprintf(pend, "%02x ", (unsigned int)data[i]);
			pend--;
			pend += sprintf(pend, "]\n");
			printf(sbuffer.get());
		}
#endif

		void JetPrinterSerial::Query(UINT8 WriteData[], UINT16 WriteCount, UINT8 ReadData[], PUINT32 ReadCount)
		{
			CAutoLock al(m_JetPrinterSerialLock);

			unsigned int ReadDataAlloc = *ReadCount;
			*ReadCount = 0;

			//Header
			vector<UINT8> wdata(WriteCount + 3);
			unsigned int wdataSize = 0;

			wdata[0] = '^';
			wdata[1] = '0';

			int i;
			for (i = 0; i < WriteCount; i++)
			{
				wdata[i + 2] = WriteData[i];
			}
			i += 2;
			wdata[i++] = 0x0D; //<CR>
			wdataSize = i;

			if (wdataSize > 2048)
			{
				char msg[256];
				sprintf_s(msg, sizeof(msg), "Message too big >2048bytes, actual size: %d bytes", wdataSize);
				throw JetPrinterExc(GetCOMPort(), msg);
			}

			vector<UINT8> rdata(ReadDataAlloc);
			unsigned int rdataSize = ReadDataAlloc;

#ifdef JETPRINETRSERIAL_LOG
			SYSTEMTIME wt;
			GetSystemTime(&wt);
			Log(GetCOMPort(), "W", wdata.get(), wdataSize, wt);
#endif
			try
			{
				Serial::SetTermChar(true, 0x0D);
				Serial::Query(wdata.data(), wdataSize, rdata.data(), &rdataSize);
			}
			catch (char* &e)
			{
				char msg[256];
				sprintf_s(msg, sizeof(msg), "%s", (const char*)e);
				throw JetPrinterExc(GetCOMPort(), msg);
			}

#ifdef JETPRINETRSERIAL_LOG
			SYSTEMTIME rt;
			GetSystemTime(&rt);
			Log(GetCOMPort(), "R", rdata.get(), rdataSize, rt);
#endif

			if (rdataSize < 10 || rdata[0] != '^' || rdata[7] != '=')
			{
				char msg[256];
				sprintf_s(msg, sizeof(msg), "JetPrinter no response");
				throw JetPrinterExc(GetCOMPort(), msg);
			}

			//response length check ^000023=EX
			int responseLength = strtoul((char*)&rdata[1], NULL, 10);
			if (responseLength == (rdataSize - 7))
			{
				memcpy(ReadData, &rdata[10], rdataSize - 10);
				*ReadCount = rdataSize - 10;
			}
			else
			{
				char msg[256];
				sprintf_s(msg, sizeof(msg), "JetPrinter response length error; expected: %d, real: %d", responseLength, (rdataSize-7));
				throw JetPrinterExc(GetCOMPort(), msg);
			}
		}

		void JetPrinterSerial::Write(UINT8 WriteData[], UINT16 WriteCount)
		{
			CAutoLock al(m_JetPrinterSerialLock);

			//Header
			vector<UINT8> wdata(WriteCount + 3);
			unsigned int wdataSize = 0;

			wdata[0] = '^';
			wdata[1] = '0';

			int i;
			for (i = 0; i<WriteCount; i++)
			{
				wdata[i + 2] = WriteData[i];
			}
			i += 2;
			wdata[i++] = 0x0D; //<CR>
			wdataSize = i;

			if (wdataSize>2048)
			{
				char msg[256];
				sprintf_s(msg, sizeof(msg), "Message too big >2048bytes, actual size: %d bytes", wdataSize);
				throw JetPrinterExc(GetCOMPort(), msg);
			}

#ifdef JETPRINETRSERIAL_LOG
			SYSTEMTIME wt;
			GetSystemTime(&wt);
			Log(GetCOMPort(), "W", wdata.get(), wdataSize, wt);
#endif
			try
			{
				Serial::Write(wdata.data(), wdataSize);
			}
			catch (char* &e)
			{
				char msg[256];
				sprintf_s(msg, sizeof(msg), "%s", (const char*)e);
				throw JetPrinterExc(GetCOMPort(), msg);
			}
		}
	}
}