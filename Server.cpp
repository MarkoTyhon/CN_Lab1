#include <iostream>
#include <WS2tcpip.h>
#include <string>
#include <ctime>
#include <Windows.h>


#pragma comment (lib, "ws2_32.lib")



int a = 0;
int a_time = time(NULL);
int b = 0;
int b_time = time(NULL);
int c = 0;
int c_time = time(NULL);


void chtEnd(SOCKET clientSocket) {
	std::string end = "end";
	Sleep(1);
	send(clientSocket, end.c_str(), sizeof(end), NULL);
}


void synchServerPartOne(SOCKET clientSocket) {
	char buf[256];

	std::string msg = "_get";
	send(clientSocket, msg.c_str(), sizeof(msg), NULL);


	int bytesReceived = recv(clientSocket, buf, 256, 0);
	int new_val = stoi(std::string(buf, 0, bytesReceived));
	bytesReceived = recv(clientSocket, buf, 256, 0);
	int new_time = stoi(std::string(buf, 0, bytesReceived));
	if (new_time > a_time) {
		a = new_val;
		a_time = time(NULL);
	}

	
	bytesReceived = recv(clientSocket, buf, 256, 0);
	new_val = stoi(std::string(buf, 0, bytesReceived));
	bytesReceived = recv(clientSocket, buf, 256, 0);
	new_time = stoi(std::string(buf, 0, bytesReceived));
	if (new_time > b_time) {
		b = new_val;
		b_time = time(NULL);
	}

	bytesReceived = recv(clientSocket, buf, 256, 0);
	new_val = stoi(std::string(buf, 0, bytesReceived));
	bytesReceived = recv(clientSocket, buf, 256, 0);
	new_time = stoi(std::string(buf, 0, bytesReceived));
	if (new_time > c_time) {
		c = new_val;
		c_time = time(NULL);
	}
}


void synchServerPartTwo(SOCKET clientSocket) {
	std::string msg = "_set";
	send(clientSocket, msg.c_str(), sizeof(msg), NULL);

	Sleep(1);
	msg = std::to_string(a);
	send(clientSocket, msg.c_str(), sizeof(msg), NULL);

	Sleep(1);
	msg = std::to_string(b);
	send(clientSocket, msg.c_str(), sizeof(msg), NULL);

	Sleep(1);
	msg = std::to_string(c);
	send(clientSocket, msg.c_str(), sizeof(msg), NULL);
}


void synch(SOCKET clientSocket) {
	synchServerPartOne(clientSocket);
	synchServerPartTwo(clientSocket);

	std::string msg = "variables synchronized";
	std::cout << msg << std::endl;
	send(clientSocket, msg.c_str(), sizeof(msg), NULL);
	chtEnd(clientSocket);
}


void serverSet(SOCKET clientSocket) {
	char buf[256];
	int int_val;
	bool stat = true;

	std::string msg = "Var(a/b/c):";
	send(clientSocket, msg.c_str(), sizeof(msg), 0);						//send msg
	chtEnd(clientSocket);

	int bytesReceived = recv(clientSocket, buf, 256, 0);					//get var
	std::string var = std::string(buf, 0, bytesReceived);
	std::cout << var << std::endl;


	msg = "Val(int):";
	send(clientSocket, msg.c_str(), sizeof(msg)+1, 0);                      //send buf
	chtEnd(clientSocket);

	ZeroMemory(buf, 256);
	bytesReceived = recv(clientSocket, buf, 256, 0);						//get val
	std::string val = std::string(buf, 0, bytesReceived);
	std::cout << val << std::endl;

	 
	try {
		int i = std::stoi(val);
		int_val = i;
	}
	catch (std::invalid_argument const& e) {
		std::string msg = "Bad input: std::invalid_argument thrown";
		std::cout << msg << std::endl;
		send(clientSocket, msg.c_str(), sizeof(msg) + 1, NULL);
		chtEnd(clientSocket);
		stat = false;
	}
	catch (std::out_of_range const& e) {
		std::string msg = "Integer overflow: std::out_of_range thrown";
		std::cout << msg << std::endl;
		send(clientSocket, msg.c_str(), sizeof(msg) + 1, NULL);
		chtEnd(clientSocket);
		stat = false;
	}
	if ((stat)&&(!var.empty())) {
		switch (var.at(0))
		{
		case 'a':
			a = int_val;
			a_time = time(NULL);
			break;
		case 'b':
			b = int_val;
			b_time = time(NULL);
			break;
		case 'c':
			c = int_val;
			c_time = time(NULL);
			break;

		default:
			msg = "wrong variabl";
			send(clientSocket, msg.c_str(), sizeof(msg) + 1, NULL);
			chtEnd(clientSocket);
			break;
		}
	}
	if (stat) {
		msg = "values secsesfully changed " + var + " = " + val;
		std::cout << msg << std::endl;
		send(clientSocket, msg.c_str(), sizeof(msg) + 1, NULL);
		chtEnd(clientSocket);
	}
}


void serverGet(SOCKET clientSocket) {
	char buf[256];

	std::string msg = "Var(a/b/c):";
	send(clientSocket, msg.c_str(), sizeof(msg), 0);						//send msg
	chtEnd(clientSocket);

	int bytesReceived = recv(clientSocket, buf, 256, 0);					//get var
	std::string var = std::string(buf, 0, bytesReceived);
	std::cout << var << std::endl;

	if (!var.empty()) {
		switch (var.at(0)) {
		case 'a':
			msg = "a = " + std::to_string(a);
			send(clientSocket, msg.c_str(), sizeof(msg) + 1, NULL);
			chtEnd(clientSocket);
			break;
		case 'b':
			msg = "b = " + std::to_string(b);
			send(clientSocket, msg.c_str(), sizeof(msg) + 1, NULL);
			chtEnd(clientSocket);
			break;
		case 'c':
			msg = "c = " + std::to_string(c);
			send(clientSocket, msg.c_str(), sizeof(msg) + 1, NULL);
			chtEnd(clientSocket);
			break;
		default:
			msg = "wrong variabl";
			std::cout << msg << std::endl;
			send(clientSocket, msg.c_str(), sizeof(msg) + 1, NULL);
			chtEnd(clientSocket);
			break;
		}
	}
}

void who(SOCKET clientSocket) {
	std::string msg1 = "Tykhonovskyi Marko K-23";
	std::string msg2 = "Variant-17";
	std::cout << msg1 << std::endl;
	std::cout << msg2 << std::endl;
	send(clientSocket, msg1.c_str(), sizeof(msg1) + 1, NULL);
	send(clientSocket, msg2.c_str(), sizeof(msg2) + 1, NULL);

}

void unckownComm(SOCKET clientSocket) {
	std::string msg = "unckown command!";
	std::cout << msg << std::endl;
	send(clientSocket, msg.c_str(), sizeof(msg), NULL);
	chtEnd(clientSocket);
}



void command_menu(std::string comm, SOCKET clientSocket) {

	if (comm == "serverSet") {
		serverSet(clientSocket);
	}
	else if (comm == "synch") {
		synch(clientSocket);
	}
	else if (comm == "serverGet") {
		serverGet(clientSocket);
	}
	else if (comm == "Who") {
		who(clientSocket);
	}
	else {
		unckownComm(clientSocket);
	}
}


void main(){
	// Initialze winsock
	WSADATA wsData;
	WORD ver = MAKEWORD(2, 2);

	int wsOk = WSAStartup(ver, &wsData);
	if (wsOk != 0){
		std::cerr << "Can't Initialize winsock! Quitting" << std::endl;
		return;
	}

	// Create a socket
	SOCKET listening = socket(AF_INET, SOCK_STREAM, 0);
	if (listening == INVALID_SOCKET){
		std::cerr << "Can't create a socket! Quitting" << std::endl;
		return;
	}

	// Bind the ip address and port to a socket
	sockaddr_in hint;
	hint.sin_family = AF_INET;
	hint.sin_port = htons(54000);
	hint.sin_addr.S_un.S_addr = INADDR_ANY; // Could also use inet_pton .... 

	bind(listening, (sockaddr*)&hint, sizeof(hint));

	// Tell Winsock the socket is for listening 
	listen(listening, SOMAXCONN);

	// Wait for a connection
	sockaddr_in client;
	int clientSize = sizeof(client);

	SOCKET clientSocket = accept(listening, (sockaddr*)&client, &clientSize);

	char host[NI_MAXHOST];		// Client's remote name
	char service[NI_MAXSERV];	// Service (i.e. port) the client is connect on

	ZeroMemory(host, NI_MAXHOST); // same as memset(host, 0, NI_MAXHOST);
	ZeroMemory(service, NI_MAXSERV);

	if (getnameinfo((sockaddr*)&client, sizeof(client), host, NI_MAXHOST, service, NI_MAXSERV, 0) == 0){
		std::cout << host << " connected on port " << service << std::endl;
	} else {
		inet_ntop(AF_INET, &client.sin_addr, host, NI_MAXHOST);
		std::cout << host << " connected on port " <<
			ntohs(client.sin_port) << std::endl;
	}

	// Close listening socket
	closesocket(listening);

	// While loop: accept and echo message back to client
	char buf[256];
	std::string command;

	while (true){
		ZeroMemory(buf, 256);

		// Wait for client to send data
		int bytesReceived = recv(clientSocket, buf, 256, 0);
		if (bytesReceived == SOCKET_ERROR) {
			std::cerr << "Error in recv(). Quitting" << std::endl;
			break;
		}

		if (bytesReceived == 0) {
			std::cout << "Client disconnected " << std::endl;
			break;
		}	

		std::cout <<  std::string(buf, 0, bytesReceived) << std::endl;
		 
		std::string comm = std::string(buf, 0, bytesReceived);

		command_menu(comm, clientSocket);
		
		//send(clientSocket, buf, bytesReceived + 1, 0);

	}

	// Close the socket
	closesocket(clientSocket);

	// Cleanup winsock
	WSACleanup();

	system("pause");
}
