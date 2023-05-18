#include <iostream>
#include <string>
#include <WS2tcpip.h>
#include <Windows.h>

#pragma comment(lib, "ws2_32.lib")


int a = 0;
int a_time = time(NULL);
int b = 0;
int b_time = time(NULL);
int c = 0;
int c_time = time(NULL);

std::string getString(char x)
{
	std::string s(1, x);
	return s;
}

int getGoodIntInput() {

	int res = 0;

	std::cout << "New value(int):" << std::endl;
	std::cout << "> ";
	std::cin >> res;

	while (!std::cin.good()) {
		std::cin.clear();
		std::cin.ignore();

		std::cout << "New value(int):" << std::endl;
		std::cout << "> ";
		std::cin >> res;
	}
	return res;
}

void clientSet() {
	std::string var;
	std::string val;
	bool stat = true;
	int int_val;

	std::cout << "Variabl(a/b/c):" << std::endl;
	std::cout << "> ";
	getline(std::cin, var);
	std::cout << "New value(int):" << std::endl;
	std::cout << "> ";
	getline(std::cin, val);


	try {
		int i = std::stoi(val);
		int_val = i;
	}
	catch (std::invalid_argument const& e) {
		std::cout << "Bad input: std::invalid_argument thrown" << std::endl;
		stat = false;
	}
	catch (std::out_of_range const& e) {
		std::cout << "Integer overflow: std::out_of_range thrown" << std::endl;
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
			std::cout << "wrong variabl" << std::endl;
			stat = false;
			break;
		}
	}
	if (stat){ std::cout << "values secsesfully changed " << var << " = " << std::to_string(int_val) << std::endl; }
	
}



void clientGet() {
	std::string var;
	std::cout << "Var(a/b/c):" << std::endl;
	std::cout << "> ";
	getline(std::cin, var);
	if (!var.empty()) {
		switch (var.at(0))
		{
		case 'a':
			std::cout << "a = " << a << std::endl;
			break;
		case 'b':
			std::cout << "b = " << b << std::endl;
			break;
		case 'c':
			std::cout << "c = " << c << std::endl;
			break;

		default:
			std::cout << "smt went wrong!" << std::endl;
			break;
		}
	}
	
}

void synchClientPartOne(SOCKET sock) {
	char buf[256];

	
	Sleep(1);
	std::string a_str = std::to_string(a);
	send(sock, a_str.c_str(), sizeof(a_str), NULL);
	Sleep(1);
	a_str = std::to_string(a_time);
	send(sock, a_str.c_str(), sizeof(a_str), NULL);

	Sleep(1);
	std::string b_str = std::to_string(b);
	send(sock, b_str.c_str(), sizeof(b_str), NULL);
	Sleep(1);
	b_str = std::to_string(b_time);
	send(sock, b_str.c_str(), sizeof(b_str), NULL);

	Sleep(1);
	std::string c__str = std::to_string(c);
	send(sock, c__str.c_str(), sizeof(c__str), NULL);
	Sleep(1);
	c__str = std::to_string(c_time);
	send(sock, c__str.c_str(), sizeof(c__str), NULL);
} 

void synchClientPartTwo(SOCKET sock) {
	char buf[256];

	int bytesReceived = recv(sock, buf, 256, 0);
	a = stoi(std::string(buf, 0, bytesReceived)); 
	a_time = time(NULL);

	bytesReceived = recv(sock, buf, 256, 0);
	b = stoi(std::string(buf, 0, bytesReceived));
	b_time = time(NULL);

	bytesReceived = recv(sock, buf, 256, 0);
	c = stoi(std::string(buf, 0, bytesReceived));
	c_time = time(NULL);
}

bool command_menu(std::string comm, SOCKET sock) {

	if (comm == "clientSet") {
		clientSet();
		return true;
	}
	else if (comm == "clientGet") {
		clientGet();
		return true;
	}
	else if (comm == "_get") {
		synchClientPartOne(sock);
		return true;
	}
	else if (comm == "_set") {
		synchClientPartTwo(sock);
		return true;
	}
	else {
		return false;
	}
}

void clientHandler(SOCKET sock) {
	char msg[256];
	while (true) {
		int bytesReceived = recv(sock, msg, 256, NULL);
		std::string tmp = msg;
		if ((tmp == "end") || (bytesReceived <= 0)) {
			break;
			exit(0);
		}
		if (!command_menu(msg, sock)) {
			std::cout << "SERVER> " << tmp << std::endl;
		}
		
	}
}



void main()
{
	std::string ipAddress = "127.0.0.1";			// IP Address of the server
	int port = 54000;						// Listening port # on the server

	// Initialize WinSock
	WSAData data;
	WORD ver = MAKEWORD(2, 2);
	int wsResult = WSAStartup(ver, &data);
	if (wsResult != 0)
	{
		std::cerr << "Can't start Winsock, Err #" << wsResult << std::endl;
		return;
	}

	// Create socket
	SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock == INVALID_SOCKET)
	{
		std::cerr << "Can't create socket, Err #" << WSAGetLastError() << std::endl;
		WSACleanup();
		return;
	}

	// Fill in a hint structure
	sockaddr_in hint;
	hint.sin_family = AF_INET;
	hint.sin_port = htons(port);
	inet_pton(AF_INET, ipAddress.c_str(), &hint.sin_addr);

	// Connect to server
	int connResult = connect(sock, (sockaddr*)&hint, sizeof(hint));
	if (connResult == SOCKET_ERROR)
	{
		std::cerr << "Can't connect to server, Err #" << WSAGetLastError() << std::endl;
		closesocket(sock);
		WSACleanup();
		return;
	}

	// Do-while loop to send and receive data
	char buf[256];
	std::string userInput;

	do
	{
		// Prompt the user for some text
		std::cout << "> ";
		getline(std::cin, userInput);
		if (userInput.empty()) {
			std::cout << "input cann not be empty" << std::endl;
			continue;
		}
		if (command_menu(userInput, sock)) { continue; }


		if (userInput != "exit")
		{
			// Send the text
			
			int sendResult = send(sock, userInput.c_str(), userInput.size() + 1, 0);
			if (sendResult != SOCKET_ERROR)
			{
				// Wait for response
				clientHandler(sock);
			}
		}
	

	} while (userInput != "exit");

	// Gracefully close down everything
	closesocket(sock);
	WSACleanup();
}
