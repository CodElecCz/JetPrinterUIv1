#pragma once

#include <WinSock2.h>
#include "stdafx.h"

using namespace std;

namespace Utilities
{
	namespace OMC8000
	{
		class UdpProtocol
		{
		public:
			UdpProtocol(void);
			UdpProtocol(const char* IPadress, UINT16 LocalPort = 57999, UINT16 RemotePort = 57999);
			~UdpProtocol(void);

		public:
			bool IsInitialized(void) { return m_WSAStartup; };

			void Connect(const char* IPaddress, UINT16 LocalPort = 57999);
			bool IsConnected() { return m_Socket != INVALID_SOCKET; };
			void Disconnect();

			void Send(UINT8 WriteData[], UINT32 WriteCount);
			void Receive(UINT8 ReadData[], PUINT32 ReadCount);
			void Query(UINT8 WriteData[], UINT32 WriteCount, UINT8 ReadData[], PUINT32 ReadCount);

			const char* GetIPaddress() { return m_IPaddress.c_str(); };
			int GetTimeout() { return m_Timeout; };

		private:
			void socket_send(UINT8 WriteData[], UINT32 WriteCount);
			void socket_receive(UINT8 ReadData[], PUINT32 ReadCount);

		private:
			SOCKET m_Socket;
			bool m_WSAStartup;
			string m_IPaddress;
			UINT16 m_LocalPort;
			UINT16 m_RemotePort;
			int m_Timeout;
			CLock m_SocketLock;

		};
	}
}
