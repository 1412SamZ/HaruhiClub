

#define sperror(x, str) if(x==-1)std::cout << str << " error" << std::endl;

#include<sstream>
#include"include/mySocket.h"

int main(int argc, char** argv) {

	std::string ip = "124.223.159.44";
	port_t port = 9999;
	TcpSocket* ts = new TcpSocket();

	ts -> toHost(ip, port);
	//std::string recvStr;
	//recvStr = ts->recvMsg();
	//std::cout << recvStr << std::endl;
	std::string inputLine;
	std::string output;
	while(1)
	{
		std::getline(std::cin, inputLine);
		ts->sendMsg(inputLine);
		// ts->recvMsg();
		char* buf = new char[100];
		auto r = ::recv(ts->getConnectFd(), buf, 100, 0);
		std::string ssss = (buf+9+40+1+8);
		std::cout << ssss;
		delete buf;
		// printf("%s\n", output.c_str());
	}
	

	return 0;

}