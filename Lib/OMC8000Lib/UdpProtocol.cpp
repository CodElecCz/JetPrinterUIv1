#include "stdafx.h"
#include "UdpProtocol.h"

namespace Utilities
{
	namespace OMC8000
	{
#define TIMEOUT 400

		UdpProtocol::UdpProtocol(void)
		{
			m_Socket = INVALID_SOCKET;
			m_IPaddress = string("127.0.0.1");
			m_LocalPort = 57999;
			m_RemotePort = 57999;
			m_Timeout = TIMEOUT;
			m_WSAStartup = false;

			WSADATA wsadata;

			if (WSAStartup(MAKEWORD(2, 2), &wsadata))
			{
				char err_msg[256];
				sprintf_s(err_msg, sizeof(err_msg), "WSAStartup() error");
				throw err_msg;
			}
			else
				m_WSAStartup = true;
		}

		UdpProtocol::UdpProtocol(const char* IPaddress, UINT16 LocalPort, UINT16 RemotePort)
		{
			m_Socket = INVALID_SOCKET;
			m_IPaddress = string(IPaddress);
			m_LocalPort = LocalPort;
			m_RemotePort = RemotePort;
			m_Timeout = TIMEOUT;
			m_WSAStartup = false;

			WSADATA wsadata;

			if (WSAStartup(MAKEWORD(2, 2), &wsadata))
			{
				char err_msg[256];
				sprintf_s(err_msg, sizeof(err_msg), "WSAStartup() error");
				throw err_msg;
			}
			else
				m_WSAStartup = true;
		}

		UdpProtocol::~UdpProtocol(void)
		{
			if (m_WSAStartup)
			{
				Disconnect();
				WSACleanup();
				m_WSAStartup = false;
			}
		}

		typedef struct SWSAErrorList {
			int num;
			const char *sym;
			const char *str;
		} WSAErrorList;

		static WSAErrorList m_WSAErrorList[] = {
				-1,					"WSA",
									NULL,
				WSAEACCES,			"EACCES",
									"Permission denied",
				WSAEADDRINUSE,		"EADDRINUSE",
									"Address already in use",
				WSAEADDRNOTAVAIL,	"EADDRNOTAVAIL",
									"Cannot assign requested address",
				WSAEAFNOSUPPORT,	"EAFNOSUPPORT",
									"Address family not supported by protocol family",
				WSAEALREADY,		"EALREADY",
									"Operation already in progress",
				WSAECONNABORTED,	"ECONNABORTED",
									"Software caused connection abort",
				WSAECONNREFUSED,	"ECONNREFUSED",
									"Connection refused",
				WSAECONNRESET,		"ECONNRESET",
									"Connection reset by peer",
				WSAEDESTADDRREQ,	"EDESTADDRREQ",
									"Destination address required",
				WSAEFAULT,			"EFAULT",
									"Bad address",
				WSAEHOSTDOWN,		"EHOSTDOWN",
									"Host is down",
				WSAEHOSTUNREACH,	"EHOSTUNREACH",
									"No route to host",
				WSAEINPROGRESS,		"EINPROGRESS",
									"Operation now in progress",
				WSAEINTR,			"EINTR",
									"Interrupted function call",
				WSAEINVAL,			"EINVAL",
									"Invalid argument",
				WSAEISCONN,			"EISCONN",
									"Udp is already connected",
				WSAEMFILE,			"EMFILE",
									"Too many open files",
				WSAEMSGSIZE,		"EMSGSIZE",
									"Message too long",
				WSAENETDOWN,		"ENETDOWN",
									"Network is down",
				WSAENETRESET,		"ENETRESET",
									"Network dropped connection on reset",
				WSAENETUNREACH,		"ENETUNREACH",
									"Network is unreachable",
				WSAENOBUFS,			"ENOBUFS",
									"No buffer space available",
				WSAENOPROTOOPT,		"ENOPROTOOPT",
									"Bad protocol option",
				WSAENOTCONN,		"ENOTCONN",
									"Udp is not connected",
				WSAENOTSOCK,		"ENOTSOCK",
									"Udp operation on non-socket",
				WSAEOPNOTSUPP,		"EOPNOTSUPP",
									"Operation not supported",
				WSAEPFNOSUPPORT,	"EPFNOSUPPORT",
									"Protocol family not supported",
				WSAEPROCLIM,		"EPROCLIM",
									"Too many processes",
				WSAEPROTONOSUPPORT,	"EPROTONOSUPPORT",
									"Protocol not supported",
				WSAEPROTOTYPE,		"EPROTOTYPE",
									"Protocol wrong type for socket",
				WSAESHUTDOWN,		"ESHUTDOWN",
									"Cannot send after socket shutdown",
				WSAESOCKTNOSUPPORT,	"ESOCKTNOSUPPORT",
									"Udp type not supported",
				WSAETIMEDOUT,		"ETIMEDOUT",
									"Connection timed out",
				WSAEWOULDBLOCK,		"EWOULDBLOCK",
									"Resource temporarily unavailable",
				WSAHOST_NOT_FOUND,	"HOST_NOT_FOUND",
									"Host not found",
				WSA_INVALID_HANDLE,	"_INVALID_HANDLE",
									"Specified event object handle is invalid",
				WSA_INVALID_PARAMETER,	"_INVALID_PARAMETER",
									"One or more parameters are invalid",
				WSAEINVALIDPROCTABLE,	"EINVALIDPROCTABLE",
									"Invalid procedure table from service provider",
				WSAEINVALIDPROVIDER,"EINVALIDPROVIDER",
									"Invalid service provider version number",
				WSA_IO_PENDING,		"_IO_PENDING",
									"Overlapped operations will complete later",
				WSA_IO_INCOMPLETE,	"_IO_INCOMPLETE",
									"Overlapped I/O event object not in signaled state",
				WSA_NOT_ENOUGH_MEMORY,	"_NOT_ENOUGH_MEMORY",
									"Insufficient memory available",
				WSANOTINITIALISED,	"NOTINITIALISED",
									"Successful WSAStartup not yet performed",
				WSANO_DATA,			"NO_DATA",
									"Valid name, no data record of requested type",
				WSANO_RECOVERY,		"NO_RECOVERY",
									"This is a non-recoverable error",
				WSAEPROVIDERFAILEDINIT,	"EPROVIDERFAILEDINIT",
									"Unable to initialize a service provider",
				WSASYSCALLFAILURE,	"SYSCALLFAILURE",
									"System call failure",
				WSASYSNOTREADY,		"SYSNOTREADY",
									"Network subsystem is unavailable",
				WSATRY_AGAIN,		"TRY_AGAIN",
									"Non-authoritative host not found",
				WSAVERNOTSUPPORTED,	"VERNOTSUPPORTED",
									"WINSOCK.DLL version out of range",
				WSAEDISCON,			"EDISCON",
									"Graceful shutdown in progress",
				WSA_OPERATION_ABORTED,	"_OPERATION_ABORTED",
									"Overlapped operation aborted",
		};

		static const char* WSAGetLastErrorString(int error)
		{
			const char* serror = "WSAErrorList unknown error";
			int size = sizeof(m_WSAErrorList) / sizeof(m_WSAErrorList[0]);
			for (int i = 0; i < size; i++)
			{
				if (m_WSAErrorList[i].num == error)
				{
					serror = m_WSAErrorList[i].str;
					break;
				}
			}

			return serror;
		}

		static const char* WSAGetLastErrorSymbol(int error)
		{
			const char* serror = "WSAErrorList unknown error";
			int size = sizeof(m_WSAErrorList) / sizeof(m_WSAErrorList[0]);
			for (int i = 0; i < size; i++)
			{
				if (m_WSAErrorList[i].num == error)
				{
					serror = m_WSAErrorList[i].sym;
					break;
				}
			}

			return serror;
		}

		void UdpProtocol::Connect(const char* IPaddress, UINT16 LocalPort)
		{
			m_Socket = socket(AF_INET, SOCK_DGRAM, 0); //Create socket
			if (m_Socket == INVALID_SOCKET)
			{
				int wsa_err = WSAGetLastError();
				const char* wsa_msg = WSAGetLastErrorString(wsa_err);
				const char* wsa_sym = WSAGetLastErrorSymbol(wsa_err);

				char err_msg[256];
				sprintf_s(err_msg, sizeof(err_msg), "socket() error: 0x%X; %s; %s", wsa_err, wsa_sym, wsa_msg);
				throw err_msg;
			}

			int optval = 1;
			setsockopt(m_Socket, SOL_SOCKET, SO_REUSEADDR, (const char*)&optval, sizeof(int));

			//set the socket in non-blocking
			unsigned long iMode = 1;
			int iResult = ioctlsocket(m_Socket, FIONBIO, &iMode);
			if (iResult == SOCKET_ERROR)
			{
				Disconnect();

				char err_msg[256];
				sprintf_s(err_msg, sizeof(err_msg), "ioctlsocket(FIONBIO) error: %d", iResult);
				throw err_msg;
			}

			//Fill out the information needed to initialize a socket…
			SOCKADDR_IN server; //Udp address information
			server.sin_family = AF_INET; // address family Internet
			server.sin_port = htons(LocalPort); //Port to connect on
			server.sin_addr.s_addr = htonl(INADDR_ANY); //Target IP

			iResult = ::bind(m_Socket, (SOCKADDR *)&server, sizeof(server));
			if (iResult == SOCKET_ERROR)
			{
				int wsa_err = WSAGetLastError();

				{
					Disconnect();

					const char* wsa_msg = WSAGetLastErrorString(wsa_err);
					const char* wsa_sym = WSAGetLastErrorSymbol(wsa_err);

					char err_msg[256];
					sprintf_s(err_msg, sizeof(err_msg), "bind() error: 0x%X; %s; %s", wsa_err, wsa_sym, wsa_msg);
					throw err_msg;
				}
			}
		}

		void UdpProtocol::Disconnect()
		{
			if (m_Socket != INVALID_SOCKET)
			{
				closesocket(m_Socket);
				m_Socket = INVALID_SOCKET;
			}
		}

		void UdpProtocol::socket_send(UINT8 WriteData[], UINT32 WriteCount)
		{
			//Fill out the information needed to initialize a socket…
			SOCKADDR_IN target; //Udp address information
			target.sin_family = AF_INET; // address family Internet
			target.sin_port = htons(m_RemotePort); //Port to connect on
			target.sin_addr.s_addr = inet_addr(m_IPaddress.c_str()); //Target IP

			//send
			int iResult = sendto(m_Socket, (const char*)WriteData, WriteCount, 0, (SOCKADDR *)&target, sizeof(target));
			if (iResult == SOCKET_ERROR)
			{
				int wsa_err = WSAGetLastError();
				const char* wsa_msg = WSAGetLastErrorString(wsa_err);
				const char* wsa_sym = WSAGetLastErrorSymbol(wsa_err);

				char err_msg[256];
				sprintf_s(err_msg, sizeof(err_msg), "send() error: 0x%X; %s; %s", wsa_err, wsa_sym, wsa_msg);
				throw err_msg;
			}
		}

		void UdpProtocol::Send(UINT8 WriteData[], UINT32 WriteCount)
		{
			CAutoLock al(m_SocketLock);

			if (!IsConnected())
				Connect(m_IPaddress.c_str(), m_LocalPort);

			//send
			socket_send(WriteData, WriteCount);
		}

		void UdpProtocol::socket_receive(UINT8 ReadData[], PUINT32 ReadCount)
		{
			unsigned int ReadCountAlloc = *ReadCount;
			if (ReadCount) *ReadCount = 0;
			clock_t t = clock();
			UINT32 elapsed = 0;

			//receive
			unsigned char rdata[512];
			unsigned int readCount = 0;
			int iResult;
			do
			{
				iResult = recv(m_Socket, (char*)rdata, sizeof(rdata), 0);
				if (iResult == SOCKET_ERROR)
				{
					Sleep(1);
#if 0
					int wsa_err = WSAGetLastError();
					const char* wsa_msg = WSAGetLastErrorString(wsa_err);
					const char* wsa_sym = WSAGetLastErrorSymbol(wsa_err);

					char err_msg[256];
					sprintf_s(err_msg, sizeof(err_msg), "recv() error: 0x%X; %s; %s", wsa_err, wsa_sym, wsa_msg);

					Disconnect();

					throw err_msg;
#endif
				}
				else if (iResult == 0)
				{
					char err_msg[256];
					sprintf_s(err_msg, sizeof(err_msg), "recv() no data");
					throw err_msg;
				}
				else
				{
					if (readCount < ReadCountAlloc)
					{
						int restOfReadBuffer = ReadCountAlloc - readCount;
						int sizeToCopy = (restOfReadBuffer < iResult) ? restOfReadBuffer : iResult;
						memcpy(&ReadData[readCount], rdata, sizeToCopy);
					}
					readCount += iResult;

					if (ReadCount) *ReadCount = readCount;

					//TODO: check response 
					break;
				}

				//calculate next wait
				elapsed = (UINT32)(clock() - t);

			} while (elapsed<m_Timeout);

			if (elapsed >= m_Timeout)
			{
				char err_msg[256];
				sprintf_s(err_msg, sizeof(err_msg), "recv() timeout %d [ms]", m_Timeout);
				throw err_msg;
			}

			if (ReadCount) *ReadCount = readCount;
		}

		void UdpProtocol::Receive(UINT8 ReadData[], PUINT32 ReadCount)
		{
			CAutoLock al(m_SocketLock);

			if (!IsConnected())
				Connect(m_IPaddress.c_str(), m_LocalPort);

			//send
			socket_receive(ReadData, ReadCount);
		}

		void UdpProtocol::Query(UINT8 WriteData[], UINT32 WriteCount, UINT8 ReadData[], PUINT32 ReadCount)
		{
			CAutoLock al(m_SocketLock);

			if (!IsConnected())
				Connect(m_IPaddress.c_str(), m_LocalPort);

			//send
			socket_send(WriteData, WriteCount);

			//receive
			socket_receive(ReadData, ReadCount);
		}
	}
}
	