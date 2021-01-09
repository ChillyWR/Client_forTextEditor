#pragma once
#include "AssistClsHeader.h"

#pragma comment(lib, "ws2_32.lib")
#include <WS2tcpip.h>

#include <iostream>
#include <future>
#include <chrono>
#include <sstream>
#define locIP "127.0.0.1"

class ClientConnection : private TextRedactor {
protected:
	static std::future<void> m_Future;
	struct ConnectExceptions {
		static constexpr char SockLibNotFound[] = "Error: lib 'ws2_32.lib' not loaded";
		static constexpr char NoConnectionWServer[] = "Error: no connection with server";
		static constexpr char UnknownPacketHeader[] = "Error: unknown packet header";
		static constexpr char UnknownCommand[] = "Error: unknown cmmand";
		static constexpr char RecoverError[] = "Error: failed to recover a message";
	};
	struct Commands {
		static constexpr char connect[] = "connect";
		static constexpr char write[] = "write";
		static constexpr char text[] = "text";
		static constexpr char output[] = "output";
		static constexpr char who[] = "who";
		static constexpr char disconnect[] = "disconnect";
		static constexpr char exit[] = "exit";
	};
	enum class PacketHeader {
		DataTypeMsg,
		InfoTypeMsg,
		ErrorTypeMsg,
		CommandTypeMsg
	};
private:
	struct sockaddr_in m_IPv4Addr;
	int m_IPAddr_Size = sizeof(m_IPv4Addr);
	SOCKET m_ConnectSock;
	bool m_Connected;
	struct m_MsgType {
		PacketHeader type;
		char* command;
		char* data;
		size_t size;
	};

	void libCheck();
	void addrSetter();
	void commandLine();
	inline void clStartUp();
	void commandWaiting();
	void connectWServer();
	void messageRecv();
	void messageSender(PacketHeader, std::string, std::string = "NULL");
	void respondWaiting();
	m_MsgType& msgDescriptor(m_MsgType&, char*);
	void inline packetManeger(const m_MsgType&);
public:
	ClientConnection();
};

void inline output(std::string);
