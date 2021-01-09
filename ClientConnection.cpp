#include "ClientConnection.h"

std::future<void> ClientConnection::m_Future;

void ClientConnection::libCheck() {
	WSAData wsaData;
	WORD DLLVersion = MAKEWORD(2, 1);
	if (WSAStartup(DLLVersion, &wsaData) != 0)
		throw ConnectExceptions::SockLibNotFound;
}
void ClientConnection::addrSetter() {
	inet_pton(AF_INET, locIP, &m_IPv4Addr.sin_addr.s_addr);
	m_IPv4Addr.sin_port = htons(2026);
	m_IPv4Addr.sin_family = AF_INET;
}
void ClientConnection::connectWServer() {
	m_ConnectSock = socket(AF_INET, SOCK_STREAM, 0);
	if (connect(m_ConnectSock, (SOCKADDR*)&m_IPv4Addr, m_IPAddr_Size) != 0)
		throw ConnectExceptions::NoConnectionWServer;
	output("Connected. Check redocor.txt");
}
void ClientConnection::commandLine() {
	std::string command;
	while (true) {
		output("Avaliable commands: 'connect', 'exit'");
		std::getline(std::cin, command);
		if (strcmp(command.c_str(), Commands::connect) == 0)
			clStartUp();
		else if (strcmp(command.c_str(), Commands::exit) == 0) exit(0);
		else output("Invalid command");
	}
}
void ClientConnection::clStartUp() {
	connectWServer();
	m_Connected = true;
	m_Future = std::async(std::launch::async, &ClientConnection::respondWaiting, this);
	commandWaiting();
}
void ClientConnection::commandWaiting() {
	output("Avaliable commands: 'write', 'text', 'who', 'disconnect', 'exit'");
	std::string command;
	while (true) {
		std::getline(std::cin, command);
		if (strcmp(command.c_str(), Commands::write) == 0)
			messageSender(PacketHeader::DataTypeMsg, Commands::write, TextRedactor::getText());
		else if (strcmp(command.c_str(), Commands::text) == 0)
			output(TextRedactor::getText());
		else if (strcmp(command.c_str(), Commands::who) == 0)
			messageSender(PacketHeader::CommandTypeMsg, Commands::who);
		else if (strcmp(command.c_str(), Commands::disconnect) == 0) {
			messageSender(PacketHeader::CommandTypeMsg, Commands::disconnect);
			m_Connected = false;
			break;
		}
		else if (strcmp(command.c_str(), Commands::exit) == 0) {
			messageSender(PacketHeader::CommandTypeMsg, Commands::disconnect);
			m_Connected = false;
			closesocket(m_ConnectSock);
			exit(0);
		}
		else output("Invalid command.");
	}
}
void ClientConnection::messageSender(PacketHeader msgType, std::string command, std::string data) {
	std::string msg = std::to_string((int)msgType) + " " + command + " " + data;
	size_t size = msg.size();
	send(m_ConnectSock, (char*)&size, sizeof(size_t), 0);
	send(m_ConnectSock, msg.c_str(), size, 0);
}
void ClientConnection::respondWaiting() {
	while (m_Connected) messageRecv();
}
void ClientConnection::messageRecv() {
	m_MsgType msgInst{};
	int recived = recv(m_ConnectSock, (char*)&msgInst.size, sizeof(size_t), 0);
	if (recived == SOCKET_ERROR) {
		throw ConnectExceptions::RecoverError;
	}
	else if (recived == 0) {
		closesocket(m_ConnectSock);
	}
	else {
		char* msg = new char[msgInst.size + 1];
		msg[msgInst.size] = '\0';
		recv(m_ConnectSock, msg, msgInst.size, 0);
		if (recived == SOCKET_ERROR)
			throw ConnectExceptions::RecoverError;
		else if (recived == 0) {
			closesocket(m_ConnectSock);
		}
		else
			packetManeger(msgDescriptor(msgInst, msg));
		delete[] msg;
	}
}
ClientConnection::m_MsgType& ClientConnection::msgDescriptor(m_MsgType& msgInst, char* msg) {
	char* newToken = NULL;
	char* token = strtok_s(msg, " ", &newToken);
	std::stringstream ss; ss << token;
	int msgType; ss >> msgType;
	msgInst.type = (ClientConnection::PacketHeader)msgType;
	token = strtok_s(NULL, " ", &newToken);
	msgInst.command = token;
	token = strtok_s(NULL, "", &newToken);
	msgInst.data = token;
	return msgInst;
}
void ClientConnection::packetManeger(const m_MsgType& msgInst) {
	if (msgInst.type == PacketHeader::DataTypeMsg) {
		if (strcmp(msgInst.command, ClientConnection::Commands::write) == 0)
			TextRedactor::writeText(msgInst.data);
		if (strcmp(msgInst.command, ClientConnection::Commands::output) == 0)
			output(msgInst.data);
	}
	else if (msgInst.type == PacketHeader::InfoTypeMsg || msgInst.type == PacketHeader::ErrorTypeMsg) {
		if (strcmp(msgInst.command, ClientConnection::Commands::output) == 0)
			output(msgInst.data);
	}
	else output(ConnectExceptions::UnknownPacketHeader);
}
ClientConnection::ClientConnection() {
	TextRedactor text;
	libCheck();
	addrSetter();
	commandLine();
}

void output(const std::string str) {
	std::cout << str << std::endl;
}