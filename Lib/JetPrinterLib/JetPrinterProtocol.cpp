#include "stdafx.h"
#include "JetPrinterProtocol.h"

namespace Utils
{
	namespace JetPrinter
	{
		typedef struct SJetPrinterState {
			int code;
			const char* desc;
		} JetPrinterState;

		static JetPrinterState m_JetPrinterNozzleState[] =
		{
			{ 0, "Invalid (Standby or Init)" },
			{ 1, "Opens" },
			{ 2, "Is open" },
			{ 3, "Closes" },
			{ 4, "Is closed" },
			{ 5, "Inter (in between)" }
		};

		static JetPrinterState m_JetPrinterMachineState[] =
		{
			{ 1, "Standby" },
			{ 2, "Initialization" },
			{ 3, "Interval or Service Panel" },
			{ 4, "Ready for Action" },
			{ 5, "Ready for Printstart" },
			{ 6, "Printing" }
		};

		const char* JetPrinterNozzleStateExplain(UINT8 code)
		{
			const char* msg = "Unknown state";
			int size = sizeof(m_JetPrinterNozzleState)/sizeof(m_JetPrinterNozzleState[0]);

			for(int i = 0; i<size; i++)
			{
				if(code== m_JetPrinterNozzleState[i].code)
				{
					msg = m_JetPrinterNozzleState[i].desc;
					break;
				}
			}

			return msg;
		}

		const char* JetPrinterMachineStateExplain(UINT8 code)
		{
			const char* msg = "Unknown state";
			int size = sizeof(m_JetPrinterMachineState) / sizeof(m_JetPrinterMachineState[0]);

			for (int i = 0; i<size; i++)
			{
				if (code == m_JetPrinterMachineState[i].code)
				{
					msg = m_JetPrinterMachineState[i].desc;
					break;
				}
			}

			return msg;
		}

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


		void JetPrinterProtocol::ScriptProcessing(const char * script)
		{
			size_t dataSize = strlen(script) + 1;
			char* data = (char*)calloc(dataSize, sizeof(char));
			strcpy_s(data, dataSize, script);

			char* context = NULL;
			char* pdata = strtok_s(data, "\r\n", &context);

			size_t wdataSize = strlen(pdata) + 2;
			vector<UINT8> wdata(wdataSize);

			while (pdata!=NULL)
			{
				if (strlen(pdata) > 0)
				{
					size_t wdataSizeNew = strlen(pdata) + 2;
					if (wdataSizeNew > wdataSize)
					{
						wdata.resize(wdataSizeNew);
						wdataSize = wdataSizeNew;
					}

					wdata[0] = '*';
					strcpy_s((char*)&wdata[1], wdataSize-1, pdata);

					switch (m_Type)
					{
					case JetPrinterProtocolTCP:
						JetPrinterTcp::Write(wdata.data(), wdataSize);
						break;
					case JetPrinterProtocolSerial:
						JetPrinterSerial::Write(wdata.data(), wdataSize);
						break;
					}
				}

				pdata = strtok_s(context, "\r\n", &context);
			}
			
			free(data);
		}

		void JetPrinterProtocol::SetCounters(int val)
		{
			UINT8 wdata[16];
			int i = 0;
			wdata[0] = '=';
			wdata[1] = 'C';
			wdata[2] = 'C';
			char snumber[32];
			sprintf_s(snumber, sizeof(snumber), "%d", val);
			for (i = 0; i < strlen(snumber); i++)
			{
				wdata[i + 3] = snumber[i];
			}
			i += 3;
			UINT16 wdataSize = i;

			switch (m_Type)
			{
			case JetPrinterProtocolTCP:
				JetPrinterTcp::Write(wdata, wdataSize);
				break;
			case JetPrinterProtocolSerial:
				JetPrinterSerial::Write(wdata, wdataSize);
				break;
			}
		}

		void JetPrinterProtocol::StartPrintReady() 
		{ 
			UINT8 wdata[16];
			int i = 0;
			wdata[i++] = '!';
			wdata[i++] = 'G';
			wdata[i++] = 'O';			
			UINT16 wdataSize = i;

			switch (m_Type)
			{
			case JetPrinterProtocolTCP:
				JetPrinterTcp::Write(wdata, wdataSize);
				break;
			case JetPrinterProtocolSerial:
				JetPrinterSerial::Write(wdata, wdataSize);
				break;
			}
		}

		void JetPrinterProtocol::StopPrintReady()
		{
			UINT8 wdata[16];
			int i = 0;
			wdata[i++] = '!';
			wdata[i++] = 'S';
			wdata[i++] = 'T';
			UINT16 wdataSize = i;

			switch (m_Type)
			{
			case JetPrinterProtocolTCP:
				JetPrinterTcp::Write(wdata, wdataSize);
				break;
			case JetPrinterProtocolSerial:
				JetPrinterSerial::Write(wdata, wdataSize);
				break;
			}
		}

		void JetPrinterProtocol::NozzleClose()
		{
			UINT8 wdata[16];
			int i = 0;
			wdata[i++] = '!';
			wdata[i++] = 'N';
			wdata[i++] = 'C';
			UINT16 wdataSize = i;

			switch (m_Type)
			{
			case JetPrinterProtocolTCP:
				JetPrinterTcp::Write(wdata, wdataSize);
				break;
			case JetPrinterProtocolSerial:
				JetPrinterSerial::Write(wdata, wdataSize);
				break;
			}
		}

		void JetPrinterProtocol::NozzleOpen()
		{
			UINT8 wdata[16];
			int i = 0;
			wdata[i++] = '!';
			wdata[i++] = 'N';
			wdata[i++] = 'O';
			UINT16 wdataSize = i;

			switch (m_Type)
			{
			case JetPrinterProtocolTCP:
				JetPrinterTcp::Write(wdata, wdataSize);
				break;
			case JetPrinterProtocolSerial:
				JetPrinterSerial::Write(wdata, wdataSize);
				break;
			}
		}

		void JetPrinterProtocol::PowerOn()
		{
			UINT8 wdata[16];
			int i = 0;
			wdata[i++] = '!';
			wdata[i++] = 'P';
			wdata[i++] = 'O';
			UINT16 wdataSize = i;

			switch (m_Type)
			{
			case JetPrinterProtocolTCP:
				JetPrinterTcp::Write(wdata, wdataSize);
				break;
			case JetPrinterProtocolSerial:
				JetPrinterSerial::Write(wdata, wdataSize);
				break;
			}
		}

		void JetPrinterProtocol::PowerOff()
		{
			UINT8 wdata[16];
			int i = 0;
			wdata[i++] = '!';
			wdata[i++] = 'P';
			wdata[i++] = 'F';
			UINT16 wdataSize = i;

			switch (m_Type)
			{
			case JetPrinterProtocolTCP:
				JetPrinterTcp::Write(wdata, wdataSize);
				break;
			case JetPrinterProtocolSerial:
				JetPrinterSerial::Write(wdata, wdataSize);
				break;
			}
		}

		void JetPrinterProtocol::UserSuspend()
		{
			UINT8 wdata[16];
			int i = 0;
			wdata[i++] = '!';
			wdata[i++] = 'U';
			wdata[i++] = 'S';
			UINT16 wdataSize = i;

			switch (m_Type)
			{
			case JetPrinterProtocolTCP:
				JetPrinterTcp::Write(wdata, wdataSize);
				break;
			case JetPrinterProtocolSerial:
				JetPrinterSerial::Write(wdata, wdataSize);
				break;
			}
		}

		void JetPrinterProtocol::UserResume()
		{
			UINT8 wdata[16];
			int i = 0;
			wdata[i++] = '!';
			wdata[i++] = 'U';
			wdata[i++] = 'R';
			UINT16 wdataSize = i;

			switch (m_Type)
			{
			case JetPrinterProtocolTCP:
				JetPrinterTcp::Write(wdata, wdataSize);
				break;
			case JetPrinterProtocolSerial:
				JetPrinterSerial::Write(wdata, wdataSize);
				break;
			}
		}

		void JetPrinterProtocol::RestartMachine()
		{
			UINT8 wdata[16];
			int i = 0;
			wdata[i++] = '!';
			wdata[i++] = 'R';
			wdata[i++] = 'M';
			UINT16 wdataSize = i;

			switch (m_Type)
			{
			case JetPrinterProtocolTCP:
				JetPrinterTcp::Write(wdata, wdataSize);
				break;
			case JetPrinterProtocolSerial:
				JetPrinterSerial::Write(wdata, wdataSize);
				break;
			}
		}

		void JetPrinterProtocol::LoadJob(const char* jobName)
		{
			UINT8 wdata[256];
			int i = 0;
			wdata[0] = '=';
			wdata[1] = 'J';
			wdata[2] = 'L';
			for (i = 0; i < strlen(jobName); i++)
			{
				wdata[i + 3] = jobName[i];
			}
			i += 3;
			UINT16 wdataSize = i;

			switch (m_Type)
			{
			case JetPrinterProtocolTCP:
				JetPrinterTcp::Write(wdata, wdataSize);
				break;
			case JetPrinterProtocolSerial:
				JetPrinterSerial::Write(wdata, wdataSize);
				break;
			}
		}

		void JetPrinterProtocol::ExternalText(const char* text)
		{
			UINT8 wdata[256];
			int i = 0;
			wdata[0] = '=';
			wdata[1] = 'E';
			wdata[2] = 'T';
			for (i = 0; i < strlen(text); i++)
			{
				wdata[i + 3] = text[i];
			}
			i += 3;
			UINT16 wdataSize = i;

			switch (m_Type)
			{
			case JetPrinterProtocolTCP:
				JetPrinterTcp::Write(wdata, wdataSize);
				break;
			case JetPrinterProtocolSerial:
				JetPrinterSerial::Write(wdata, wdataSize);
				break;
			}
		}

		void JetPrinterProtocol::QueryStatus(PUINT32 NozzleState, PUINT32 MachineState, PUINT32 Error, PUINT32 Headcover, PUINT32 ActSpeed)
		{
			UINT8 wdata[16];
			int i = 0;
			wdata[i++] = '?';
			wdata[i++] = 'R';
			wdata[i++] = 'S';
			UINT16 wdataSize = i;

			UINT8 rdata[2048];
			unsigned int rdataSize = sizeof(rdata);
			memset(rdata, 0, sizeof(rdata));

			switch (m_Type)
			{
			case JetPrinterProtocolTCP:
				JetPrinterTcp::Query(wdata, wdataSize, rdata, &rdataSize);
				break;
			case JetPrinterProtocolSerial:
				JetPrinterSerial::Query(wdata, wdataSize, rdata, &rdataSize);
				break;
			}

			//parse response
			char* pdata = (char*)rdata;
			UINT32 status[5];
			int pos = 0;
			while (pdata != NULL)
			{
				status[pos++] = strtoul(pdata, &pdata, 10);

				if (pos >= (sizeof(status)/sizeof(status[0])))
					break;

				if (pdata && *pdata == '\t')
					pdata++;
				else
					break;
			}

			if (NozzleState) *NozzleState = status[0];
			if (MachineState) *MachineState = status[1];
			if (Error) *Error = status[2];
			if (Headcover) *Headcover = status[3];
			if (ActSpeed) *ActSpeed = status[4];
		}

		void JetPrinterProtocol::QueryCounter(PUINT32 ProductCounter, PUINT32 StopCounter, PUINT32 TotalCounter)
		{
			UINT8 wdata[16];
			int i = 0;
			wdata[i++] = '?';
			wdata[i++] = 'C';
			wdata[i++] = 'C';
			UINT16 wdataSize = i;

			UINT8 rdata[2048];
			unsigned int rdataSize = sizeof(rdata);
			memset(rdata, 0, sizeof(rdata));

			switch (m_Type)
			{
			case JetPrinterProtocolTCP:
				JetPrinterTcp::Query(wdata, wdataSize, rdata, &rdataSize);
				break;
			case JetPrinterProtocolSerial:
				JetPrinterSerial::Query(wdata, wdataSize, rdata, &rdataSize);
				break;
			}

			//parse response
			char* pdata = (char*)rdata;
			UINT32 status[3];
			int pos = 0;
			while (pdata != NULL)
			{
				status[pos++] = strtoul(pdata, &pdata, 10);

				if (pos >= (sizeof(status) / sizeof(status[0])))
					break;

				if (pdata && *pdata == '\t')
					pdata++;
				else
					break;
			}

			if (ProductCounter) *ProductCounter = status[0];
			if (StopCounter) *StopCounter = status[1];
			if (TotalCounter) *TotalCounter = status[2];
		}

		void JetPrinterProtocol::QueryLoadedJob(char* JobName, size_t JobNameSize)
		{
			UINT8 wdata[16];
			int i = 0;
			wdata[i++] = '?';
			wdata[i++] = 'J';
			wdata[i++] = 'L';
			UINT16 wdataSize = i;

			UINT8 rdata[2048];
			unsigned int rdataSize = sizeof(rdata);
			memset(rdata, 0, sizeof(rdata));

			switch (m_Type)
			{
			case JetPrinterProtocolTCP:
				JetPrinterTcp::Query(wdata, wdataSize, rdata, &rdataSize);
				break;
			case JetPrinterProtocolSerial:
				JetPrinterSerial::Query(wdata, wdataSize, rdata, &rdataSize);
				break;
			}

			//parse response
			strcpy_s(JobName, JobNameSize, (char*)rdata);			
		}

		void JetPrinterProtocol::QueryExternalText(char* Text, size_t TextSize)
		{
			UINT8 wdata[16];
			int i = 0;
			wdata[i++] = '?';
			wdata[i++] = 'E';
			wdata[i++] = 'T';
			UINT16 wdataSize = i;

			UINT8 rdata[2048];
			unsigned int rdataSize = sizeof(rdata);
			memset(rdata, 0, sizeof(rdata));

			switch (m_Type)
			{
			case JetPrinterProtocolTCP:
				JetPrinterTcp::Query(wdata, wdataSize, rdata, &rdataSize);
				break;
			case JetPrinterProtocolSerial:
				JetPrinterSerial::Query(wdata, wdataSize, rdata, &rdataSize);
				break;
			}

			//parse response
			strcpy_s(Text, TextSize, (char*)rdata);
		}
	}
}