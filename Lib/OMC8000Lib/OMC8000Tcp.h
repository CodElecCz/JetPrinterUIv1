#pragma once

#include "stdafx.h"
#include "TcpProtocol.h"


namespace Utilities
{
	namespace OMC8000
	{
		class OMC8000Tcp : private TcpProtocol
		{
		public:
			OMC8000Tcp(void) : TcpProtocol(), m_Id(0) {};
			OMC8000Tcp(const char* IPaddress) : TcpProtocol(IPaddress), m_Id(0) {};
			~OMC8000Tcp(void) {};

		public:
			void Query(UINT8 WriteData[], UINT16 WriteCount, UINT8 ReadData[], PUINT32 ReadCount, UINT32 ReadCountRequest);
			bool IsInitialized(void) { return TcpProtocol::IsInitialized(); };
			const char* GetIPaddress() { return TcpProtocol::GetIPaddress(); };
			int GetTimeout() { return TcpProtocol::GetTimeout(); };

		private:
			unsigned int m_Id; //Transaction Identifier
			CLock m_OMC8000TcpLock;
		};
	}
}

