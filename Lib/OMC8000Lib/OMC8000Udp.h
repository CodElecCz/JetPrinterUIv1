#pragma once

#include "stdafx.h"
#include "UdpProtocol.h"

using namespace Utilities;

namespace Utilities
{
	namespace OMC8000
	{
		class OMC8000Udp : private UdpProtocol
		{
		public:
			OMC8000Udp(void) : UdpProtocol(), m_Id(0) {};
			OMC8000Udp(const char* IPaddress, UINT16 PortLocal, UINT16 PortRemote) : UdpProtocol(IPaddress, PortLocal, PortRemote), m_Id(0) {};
			~OMC8000Udp(void) {};

		public:
			void Query(UINT8 WriteData[], UINT16 WriteCount, UINT8 ReadData[], PUINT32 ReadCount, UINT32 ReadCountRequest);
			bool IsInitialized(void) { return UdpProtocol::IsInitialized(); };
			const char* GetIPaddress() { return UdpProtocol::GetIPaddress(); };
			int GetTimeout() { return UdpProtocol::GetTimeout(); };

		private:
			unsigned int m_Id; //Transaction Identifier
			CLock m_OMC8000UdpLock;
		};
	}
}

