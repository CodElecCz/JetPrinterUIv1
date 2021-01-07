#pragma once

#include "JetPrinterDef.h"
#include "JetPrinterSerial.h"
#include "JetPrinterTCP.h"

using namespace Utils::JetPrinter;

namespace Utils
{
	namespace JetPrinter
	{
#pragma warning(push)
#pragma warning(disable:4275)

		enum JetPrinterProtocolType
		{
			JetPrinterProtocolTCP,
			JetPrinterProtocolSerial
		};

		class JETPRINTER_API JetPrinterProtocol : public JetPrinterSerial, JetPrinterTcp
		{
		public:
			JetPrinterProtocol(const char* IPaddress)
				: JetPrinterTcp(IPaddress), m_Type(JetPrinterProtocolTCP) {};
			JetPrinterProtocol(const char* COMPort, Serial::BaudRate Baudrate, Serial::DataBits Databits = Serial::DB_8, Serial::Parity Parity = Serial::P_NONE, Serial::StopBits Stopbit = Serial::SB_ONE)
				: JetPrinterSerial(COMPort, Baudrate, Databits, Parity, Stopbit), m_Type(JetPrinterProtocolSerial) {};
			~JetPrinterProtocol(void) {};

		public:
			/*
			Script Processing:

			A script starts with:	^0*BEGINLJSCRIPT<CR>
			Ends with: 				^0*ENDLJSCRIPT<CR>
			*/

			void ScriptProcessing(const char * script);

			/*
			Actions:

			!GO Start Print-Ready
			!ST Stop Print-Ready
			!NC Nozzle Close
			!NO Nozzle Open
			!RC Reset Counter (Object Counter)
			!FA Factory Reset
			!MC Motor Step Close
			!MO Motor Step Open
			!LF Re-Load Fonts
			!OK CRC OK
			!US User Suspend (Lock of printer operation)
			!UR User Resume (Release of printer operation)
			!EQ Error Quit
			!PO Power ON (Out of Standby)
			!PF Power OFF (in Standby mode)
			!EN End of Data
			!RM Restart Machine (= Software Reset, only while Standby)
			!W0 Wysiwyg display off
			!W1 Wysiwyg display on
			!FF Flush Mail Fifo (Clearing the Mailing FIFO)
			!LN The LJ3 sends in the protocol the length
			!EM Switch on the echo mode
			!RC Reset all counters to its initial value
			*/
			void StartPrintReady();
			void StopPrintReady();

			void NozzleClose();
			void NozzleOpen();

			void PowerOn();
			void PowerOff();

			void UserSuspend();
			void UserResume();

			void RestartMachine();

			void LoadJob(const char* jobName);
			void ExternalText(const char* text);

			void SetCounters(int val);

			/*
			Inquiries:
			*/

			/*
			?RS Query status of data
				^0?RS
				^000015=RS2.5.0.0.200
				=RS;Nozzle state;State of Machine;Error;Headcover;ActSpeed
			*/
			void QueryStatus(PUINT32 NozzleState, PUINT32 MachineState, PUINT32 Error, PUINT32 Headcover, PUINT32 ActSpeed);

			/*
			?EX Report Extern-States
				^0?EX
				^000023=EX200.0.192.1.0.0.0.0   
				=EX;actual speed 20.0m/min;inputs;max speed 19.2m/min;direction;print go;;;

			?SH Report Service History
				^0?SH
				^000023=SH9355.137.1213376771

			?IC Report Input Configuration
				^0?IC
				^000015=IC0.0.0.0.0.0
				=IC;

			?HS Report Hydraulic States
				^0?HS
				^000027=HS7748.3584.44.1108.57.9.
				=HS;ActFalltime 7748ms;Act Press 3584mBar; ActSuctpump 44%; Position 1108;Mix 57;State 9

			?OC Report Output Configuration
				^0?OC
				^000019=OC2.1.3.0.0.0.0.0
			
			?MC Report Main Configuration
				^0?MC
				^000053=MC0.0.0.0.0.7500.3600.0.1025.96.79000-00104.500.555
				=MC;DirectPrint;Encoder Setting;Reserved (0);Reserved (0);Reserved (0);Set Fall Time;Set Pressure;Reserved (0);Set Visco Correction;System Frequency;Inknumber;Interval Time on;Interval Time off

			?CC Report Current Counter
				^0?CC
				^000017=CC349.0.1150641
				=CC;Product Counter;Stop;Total Print Counter
			*/
			void QueryCounter(PUINT32 ProductCounter, PUINT32 StopCounter, PUINT32 TotalCounter);
			/*
			?SC Report Service Configuration
				^0?SC
				^000012=SC6000.365

			?VS Report Version
				^0?VS
				^000100=VSFPDATV120.0.0.1.134.0.0.67.0.0.0.125.9.V2.0.0.20.Typ 1 Release.Czech.0.V60.0.26.00/T2.LJ3.307252

			?JL Query name of loaded job
				^0?JL
				^000032=JL\FFSDISK\JOBS\01_TYCKA_Z.job
			*/
			void QueryLoadedJob(char* JobName, size_t JobNameSize);
			/*
			?EL Query ErrorList
				^0?EL
				^0=EL1.64.32.18-01-05 12:59:08/00010202.18-01-05 13:59:07/00010202.18-01-05 14:59:03/00010202.18-01-05 15:59:04/00010202.18-01-05 16:59:10/00010202.18-01-05 20:01:01/00010202.18-01-06 07:27:00/00010202.18-01-06 07:59:02/00010202.18-01-06 08:59:05/00010202.18-01-06 10:59:01/00010202.18-01-06 11:59:05/00010202.18-01-06 12:59:00/00010202.18-01-06 13:36:34/00012000.18-01-06 13:36:42/00012000.18-01-06 13:37:21/00012000.18-01-06 13:38:10/00012000.18-01-06 13:59:01/00010202.18-01-06 14:16:30/00012000.18-01-06 14:16:35/00012000.18-01-06 20:01:06/00010202.18-01-06 20:59:07/00010202.18-01-08 05:01:06/00010202.18-01-08 05:59:00/00010202.18-01-08 06:59:05/00010202.18-01-08 07:59:01/00010202.18-01-08 08:59:09/00010202.18-01-08 09:59:07/00010202.18-01-08 10:59:09/00010202.18-01-08 11:59:04/00010202.18-01-08 12:59:09/00010202.18-01-08 13:59:04/00010202.18-01-08 14:59:03/00010202
				^0=EL33.64.32.18-01-08 15:59:00/00010202.18-01-08 16:59:07/00010202.18-01-08 17:59:02/00010202.18-01-08 18:59:09/00010202.18-01-08 19:59:06/00010202.18-01-09 05:01:09/00010202.18-01-09 05:59:04/00010202.18-01-09 06:59:02/00010202.18-01-09 07:59:03/00010202.18-01-09 08:59:07/00010202.18-01-09 09:59:00/00010202.18-01-09 10:27:20/00010202.18-01-09 11:42:05/00012113.18-01-09 11:42:05/00012114.18-01-09 11:42:08/00001309.18-01-09 11:47:51/00012113.18-01-09 11:47:51/00012114.18-01-09 11:54:03/00012113.18-01-09 11:54:03/00012114.18-01-09 11:55:42/00012501.18-01-09 11:58:04/00012500.18-01-09 11:58:05/00012650.18-01-14 05:11:04/00001305.18-01-18 15:43:42/00021311.18-01-18 15:44:40/00012501.18-01-19 20:11:04/00001305.18-01-20 05:11:04/00001305.18-01-20 07:53:31/00012000.18-01-20 07:53:48/00012000.18-01-20 07:59:53/00012000.18-01-20 08:00:38/00012000.00-00-00 00:00:00/00012000

			?EV Query EventList
				^0=EV2705.4500.16.18-01-17 19:21:23/0/0/6/18/3565/1024/7496/19.18-01-17 19:22:24/0/0/6/18/3558/994/7493/19.18-01-17 19:23:24/0/1/6/18/3555/956/7492/19.18-01-17 19:24:25/0/0/6/18/3551/937/7493/19.18-01-17 19:25:26/0/0/6/18/3546/921/7492/19.18-01-17 19:26:26/0/1/6/18/3542/873/7493/19.18-01-17 19:27:27/0/0/7/18/3537/858/7499/19.18-01-17 19:28:27/1/1/6/18/3587/1102/7499/19.18-01-17 19:29:28/0/0/6/18/3581/1034/7502/19.18-01-17 19:30:28/0/0/6/18/3577/997/7503/19.18-01-17 19:31:29/0/1/6/17/3572/977/7508/19.18-01-17 19:32:30/0/0/6/17/3568/933/7511/19.18-01-17 19:33:30/0/0/6/17/3563/908/7512/19.18-01-17 19:34:31/0/1/6/17/3558/885/7513/19.18-01-17 19:35:31/0/0/6/17/3554/857/7513/19.18-01-17 19:36:32/0/0/6/17/3558/1098/7512/19
				^0=EV2913.4500.16.18-01-18 06:56:33/0/0/6/38/3555/1095/7433/19.18-01-18 06:57:38/0/0/6/39/3546/963/7430/19.18-01-18 06:58:38/0/0/6/39/3543/932/7430/19.18-01-18 06:59:39/0/0/7/40/3539/901/7426/19.18-01-18 07:00:39/1/0/6/39/3578/858/7431/19.18-01-18 07:01:40/0/1/6/40/3581/1053/7432/19.18-01-18 07:02:40/0/0/6/40/3577/1013/7432/19.18-01-18 07:03:41/0/0/6/40/3572/969/7432/19.18-01-18 07:04:42/0/0/6/40/3568/931/7432/19.18-01-18 07:05:42/0/0/6/40/3563/879/7432/19.18-01-18 07:06:43/0/0/6/40/3569/1122/7433/19.18-01-18 07:07:43/0/1/6/40/3563/1063/7435/19.18-01-18 07:08:44/0/0/7/41/3558/1005/7435/19.18-01-18 07:09:44/0/0/6/42/3554/968/7431/19.18-01-18 07:10:45/0/0/6/44/3549/939/7422/19.18-01-18 07:11:46/0/0/6/44/3545/909/7416/19

			?JB Inquiry for the current print job. This inquiry is sended from host to the machine, hereupon the machine sends the job (in ^0*BEGINLJSCRIPT…. blocks)
				^0?JB
				^000032*BEGINLJSCRIPT [(V01.06.00.16)]
				^000049*JLPAR [65 1 0 0 200 0 1 10000 00:00 0 7000 0 0]
				^000019*BEGINJOB [ 0 () ]
				^000067*JOBPAR [ 95000 0 0 300 0 0 0 0 0 180000 -1 () 1 0 55000 1 0 0 0 ]
				^000075*OBJ [1 0 25 0 (ISO1_7X5)  (VOLVO) 1 0 0 0 0 1 1 0 0 0 0 0 ()  () 0 0 () ]
				^000078*OBJ [2 0 17 0 (ISO1_5X5)  (31651562) 1 0 0 0 0 1 0 0 0 0 0 0 ()  () 0 0 () ]
				^000079*OBJ [3 0 10 0 (ISO1_5X5)  (GGRFA  AA) 1 0 0 0 0 1 0 0 0 0 0 0 ()  () 0 0 () ]
				^000083*OBJ [4 0 3 0 (ISO1_5X5)  (CZECH REPUBLIC) 1 0 0 0 0 1 0 0 0 0 0 0 ()  () 0 0 () ]
				^000074*OBJ [7 81 18 0 (ISO1_5X5)  ({t}) 1 0 0 0 0 1 0 0 0 0 0 0 ()  () 0 0 () ]
				^000024*TIME [ (YY-j) 0 1 0 0]
				^000060*RPLDAY [ (MON)  (TUE)  (WED)  (THU)  (FRI)  (SAT)  (SUN) ]
				^000083*RPLMON [ (01)  (02)  (03)  (04)  (05)  (06)  (07)  (08)  (09)  (10)  (11)  (12) ]
				^000169*RPLYEAR [ 2014 (2014)  (2015)  (2016)  (2017)  (2018)  (2019)  (2020)  (2021)  (2022)  (2023)  (2024)  (2025)  (2026)  (2027)  (2028)  (2029)  (2030)  (2031)  (2032) ]
				^000074*OBJ [8 97 11 0 (ISO1_5X5)  ({t}) 1 0 0 0 0 1 0 0 0 0 0 0 ()  () 0 0 () ]
				^000021*TIME [ (j) 0 1 0 0]
				^000060*RPLDAY [ (MON)  (TUE)  (WED)  (THU)  (FRI)  (SAT)  (SUN) ]
				^000083*RPLMON [ (01)  (02)  (03)  (04)  (05)  (06)  (07)  (08)  (09)  (10)  (11)  (12) ]
				^000169*RPLYEAR [ 2014 (2014)  (2015)  (2016)  (2017)  (2018)  (2019)  (2020)  (2021)  (2022)  (2023)  (2024)  (2025)  (2026)  (2027)  (2028)  (2029)  (2030)  (2031)  (2032) ]
				^000084*OBJ [9 120 10 0 (ISO1_5X5)  (1001KS180027) 1 0 0 0 0 1 0 0 0 0 0 0 ()  () 0 0 () ]
				^000011*ENDJOB []
				^000016*ENDLJSCRIPT []

			?SM GetMailStatus

			?DL Query date/time
				^0?DL
				^000023=DL2018.1.20.6.8.30.35

			?GW Query Wysiwyg

			?ET Extern Text
				^0?ET
				^000004=ET
			*/
			void QueryExternalText(char* Text, size_t TextSize);
			

			/*
			File Transfer:

			$RD Request-Directory
			$DI Directory-Info
			$RF Request File
			$FH File Header
			$FT File Transfer

			Data carrier Memory type Path:

			Internal Flash Disk:
				Jobs: FFSDISK\Jobs
				Fonts: FFSDISK\Fonts
				Graphics: FFSDISK\Graphics
			SD Card:
				Jobs: Storage Card\LJ3\Jobs
				Fonts: Storage Card\LJ3\Fonts
				Graphics: Storage Card\LJ3\Graphics
			USB Stick:
				Jobs: Hard disk\LJ3\Jobs
				Fonts: Hard disk\LJ3\Fonts
				Graphics: Hard disk\LJ3\Graphics
			*/

		private:
			;

		private:
			JetPrinterProtocolType m_Type;
		};
#pragma warning(pop)
	}
}


