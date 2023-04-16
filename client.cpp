

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

		v2msg msg("client", inputLine);

		ts->SendMsgV2(msg, ts->getConnectFd());
		std::cout << "msg sent" << std::endl;
		
		continue;
		int code;
		std::string str = ts->recvMsgV2(ts -> getConnectFd(), &code);
		if(code == 0)break;
		std::cout << "mess from server: " << str << std::endl;
		
	}
	

	return 0;

}