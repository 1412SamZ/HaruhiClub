#pragma once

#ifndef __MY_SOCKET_H__
#define __MY_SOCKET_H__
#if __linux__
#include <unistd.h>  //close 和 shutdown
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h> //inet_ntop
#include <string.h>

#define SET_IPv4(ip, strtAd) inet_pton(ipv4, ip.data(), &strtAd.wsAd);

#define ipv4 AF_INET
#define wsAd sin_addr.s_addr
#define wsF sin_family
#define wsP sin_port
#define sitos (struct sockaddr*)

typedef unsigned int ADDRLEN;


#define CLOSE_SOCKET(x) ::close(x);


#elif _WIN32
#include <Ws2tcpip.h>

#include <winsock.h>
#pragma comment(lib,"ws2_32.lib")


#define CLOSE_SOCKET(x) closesocket(x);


class WSAInit {
public:
    WSAInit() {
        WSADATA wsaData;
        if (WSAStartup(MAKEWORD(2, 2), &wsaData))
        {
            perror("WSA Initialization Failed! ");
            exit(1);
        }
    }

    ~WSAInit() {
        WSACleanup();
    }
};

static WSAInit wsa;

#define ipv4 AF_INET
#define wsAd sin_addr.S_un.S_addr
#define wsF sin_family
#define wsP sin_port
#define sitos (struct sockaddr*)

//#define SET_IPv4(ip, strtAd) inet_pton(ipv4, ip.data(), &strtAd.wsAd);

#define SET_IPv4(str_ip, strtAd)strtAd.wsAd = inet_addr(str_ip.c_str());

typedef int ADDRLEN;


#endif


#include<iostream>
#include <string>
#include <cassert>



#define CREATESOCKETv4() socket(ipv4, SOCK_STREAM, 0);


#define STRPRT_EQ_M1(x, str) if(x == -1)std::cout << str << std::endl;
#define STRPRT_EQ_ZR(x, str) if(x == 0)std::cout << str << std::endl;
#define STRPRT(str)std::cout << str << std::endl;
#define RET_EQ_M1(x, ret) if(x == -1)return ret;
typedef unsigned short U16;
typedef unsigned short port_t;

typedef unsigned int U32;

#define LSTN_MAX 128
#define LSTN_MIN 1
#define DATA_BUFF_SIZE 4096

// exp..al
#define edl + "\n"
#define errmsg "Error: " +

// v2msg define
#define REQUEST_REGISTER 1
#define REQUEST_LOGIN 2
#define MSGTYPE_ROOM 1
#define MSGTYPE_PRIVATE 2





/*
windows socket流程
1. 加入winsock.h，连接ws2lib,
2. 初始wsa startup
3. 和linux相似的使用
4. 结束wsa

x. 可以创建一个类，为wsa，析构函数写close

*/





/*
套接字通信，两部分函数
服务器，客户端，同时也有两部分共通函数

共通部分：
建立socket
关闭socket
发送 接收msg
read write

服务器：
绑定，监听套接字（监听客户端），通信套接字

客户端：
连接服务器（和服务器通信对接）


*/



// 封装
typedef struct MyMsgV2
{
    unsigned long header;
    char requestType;
    char username[40];
    char msgType;
    unsigned long msgLength;
    const char* msg;
    MyMsgV2(): header(0)
    {
        std::string msgStructStr = "DEFAULT_USER";
        memset(username, 0, sizeof username);
        for(int i = 0; i < msgStructStr.size(); i++)
        {
            username[i] = msgStructStr[i];
        }
    };
    MyMsgV2(std::string userName): header(0)
    {
        
        memset(username, 0, sizeof username);
        for(int i = 0; i < userName.size(); i++)
        {
            username[i] = userName[i];
        }
    };

    MyMsgV2(const std::string& userName, const std::string& msgToSend): header(0), msg(msgToSend.c_str())
    {
        
        memset(username, 0, sizeof username);
        for(int i = 0; i < userName.size(); i++)
        {
            username[i] = userName[i];
        }
        msgLength = msgToSend.size();
        // msg.msg = msgStructStr.c_str();

        // header
            header = sizeof(requestType)
            + sizeof(username)
            + sizeof(msgType)
            + sizeof(msgLength)
            + msgLength;
    };
}v2msg;

class TcpSocket
{
public:
    TcpSocket();
    ~TcpSocket();
    TcpSocket(int socket);
    int toHost(std::string ip, port_t port);
    int getConnectFd();
    bool sendMsg(const std::string& msg);
    std::string recvMsg(size_t buffer_size);

    bool SendMsgV2(const v2msg& msgStruct, int cfd);
    std::string recvMsgV2();

private:
    std::string recv(size_t buffer_size);
    bool send(const std::string& msg);

private:
    int connectFd;
};

TcpSocket::TcpSocket()
{
    connectFd = socket(ipv4, SOCK_STREAM, 0);
}

TcpSocket::TcpSocket(int socket)
{
    connectFd = socket;
}

TcpSocket::~TcpSocket()
{
    if (connectFd > 0)CLOSE_SOCKET(connectFd);
}

int TcpSocket::toHost(std::string ip, port_t port)
{
    struct sockaddr_in saddr;
    saddr.wsF = ipv4;
    saddr.wsP = htons(port);
    SET_IPv4(ip, saddr);

    int ret = connect(connectFd, sitos & saddr, sizeof(saddr));

    STRPRT_EQ_M1(ret, "Error: connect failed");
    RET_EQ_M1(ret, -1);

    STRPRT("Connected!");

    return ret;

}

int TcpSocket::getConnectFd()
{
    return connectFd;
}


bool TcpSocket::send(const std::string& msg) {

    return ::send(connectFd, msg.c_str(), msg.size(), 0) > 0;

}

std::string TcpSocket::recv(size_t buffer_size = DATA_BUFF_SIZE) {
    // 分配空间
    char* buf = new char[buffer_size];
    if (::recv(connectFd, buf, buffer_size, 0) < 0)
    {
        delete[] buf;
        std::cout << "Error: recv failed" << std::endl;
        return "";
    }

    std::string msg = buf;
    delete[] buf;
    return msg;
}




bool TcpSocket::sendMsg(const std::string& msg) {
    // 发送两个信息，一个长度一个data
    bool ret = ::send(connectFd, msg.c_str(), msg.size() + 1, 0) > 0;
    return ret;
}

std::string TcpSocket::recvMsg(size_t buffer_size = DATA_BUFF_SIZE) {
    std::string msg = recv(buffer_size);
    return msg;
}


// typedef struct MyMsgV2
// {
//     unsigned long header;
//     char requestType;
//     char username[40];
//     char msgType;
//     unsigned long msgLength;
//     char* msg;
// }v2msg;







bool TcpSocket::SendMsgV2(const v2msg& msg, int cfd)
{
    // header 4B
    // request type 1B -> login? msg? function?
    // username 40B
    // msg type 1B -> private? chatroom?
    // msg length 4B 
    // msg ?B

    // typedef struct MyMsgV2
    // {
    //     unsigned long header;
    //     char requestType;
    //     char username[40];
    //     char msgType;
    //     unsigned long msgLength;
    //     char* msg;
    // }v2msg;

    std::string msgBody = msg.msg;

    char* data = (char*)malloc(
        sizeof(msg.header)
        + sizeof(msg.requestType)
        + sizeof(msg.username)
        + sizeof(msg.msgType)
        + sizeof(msg.msgLength)
        + msg.msgLength
    );
    unsigned long bigInt = htonl(msg.header);
    int dataOffset = 0;

    memcpy(data + dataOffset, &bigInt, sizeof(bigInt));
    dataOffset += sizeof(bigInt);

    memcpy(data + dataOffset, &msg.requestType, sizeof(msg.requestType));
    dataOffset += sizeof(msg.requestType);

    memcpy(data + dataOffset, &msg.username, sizeof(msg.username));
    dataOffset += sizeof(msg.username);

    memcpy(data + dataOffset, &msg.msgType, sizeof(msg.msgType));
    dataOffset += sizeof(msg.msgType);

    memcpy(data + dataOffset, &msg.msgLength, sizeof(msg.msgLength));
    dataOffset += sizeof(msg.msgLength);

    // for(int i = 0; i < )

    // auto cstr = msg.msg.c_str();
    memcpy(data + dataOffset, msg.msg, msg.msgLength);
    dataOffset += msg.msgLength;
    std::string dbg = data;

    ::send(cfd, data, dataOffset, 0);

    



    return true;
}
std::string TcpSocket::recvMsgV2()
{
    std::string retStr {};
    unsigned long bufSize = std::stoi(recv());
    retStr = recv();


    return retStr;
}







class serverTCP
{
    // server只负责监听
    // 原本通信部分和客户端部分功能重叠，使用另外一个类
public:
    serverTCP();
    ~serverTCP();
    int stcpListen(port_t port);
    TcpSocket* stcpAccept(struct sockaddr_in* saddr = nullptr);
    int getListenFd();

private:
    int listenFd;
};

// define
serverTCP::serverTCP()
{
    listenFd = socket(ipv4, SOCK_STREAM, 0);
}

serverTCP::~serverTCP() {
    CLOSE_SOCKET(listenFd);
}

int serverTCP::getListenFd()
{
    return listenFd;
}

int serverTCP::stcpListen(port_t port)
{
    struct sockaddr_in saddr;
    saddr.wsF = ipv4;
    saddr.wsP = htons(port);
    saddr.wsAd = INADDR_ANY;

    int ret = bind(listenFd, sitos & saddr, sizeof(saddr));

    STRPRT_EQ_M1(ret, "Error: bind failed");
    STRPRT_EQ_ZR(ret, "Server Binded");

    ret = listen(listenFd, LSTN_MAX);

    STRPRT_EQ_M1(ret, "Error: listen failed");
    STRPRT_EQ_ZR(ret, "Listening...");

    return ret;


}

TcpSocket* serverTCP::stcpAccept(sockaddr_in* saddr)
{
    if (saddr == NULL)return nullptr;

    ADDRLEN addrlen = sizeof(struct sockaddr_in);

    int connectFd = accept(listenFd, sitos & saddr, &addrlen);

    STRPRT_EQ_M1(connectFd, "Error: accept failed");

    RET_EQ_M1(connectFd, nullptr);

    STRPRT("Connected! ");

    // 返回由cfd来new的通信类
    return new TcpSocket(connectFd);



}


#endif

