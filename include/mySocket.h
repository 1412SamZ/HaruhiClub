#pragma once

#ifndef __MY_SOCKET_H__
#define __MY_SOCKET_H__
#if __linux__
#include <unistd.h> //close 和 shutdown
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
#define sitos (struct sockaddr *)

typedef unsigned int ADDRLEN;

#define CLOSE_SOCKET(x) ::close(x);

#elif _WIN32
#include <Ws2tcpip.h>

#include <winsock.h>
#pragma comment(lib, "ws2_32.lib")

#define CLOSE_SOCKET(x) closesocket(x);

class WSAInit
{
public:
    WSAInit()
    {
        WSADATA wsaData;
        if (WSAStartup(MAKEWORD(2, 2), &wsaData))
        {
            perror("WSA Initialization Failed! ");
            exit(1);
        }
    }

    ~WSAInit()
    {
        WSACleanup();
    }
};

static WSAInit wsa;

#define ipv4 AF_INET
#define wsAd sin_addr.S_un.S_addr
#define wsF sin_family
#define wsP sin_port
#define sitos (struct sockaddr *)

// #define SET_IPv4(ip, strtAd) inet_pton(ipv4, ip.data(), &strtAd.wsAd);

#define SET_IPv4(str_ip, strtAd) strtAd.wsAd = inet_addr(str_ip.c_str());

typedef int ADDRLEN;

#endif

#include <iostream>
#include <string>
#include <cassert>

#define CREATESOCKETv4() socket(ipv4, SOCK_STREAM, 0);

#define STRPRT_EQ_M1(x, str) \
    if (x == -1)             \
        std::cout << str << std::endl;
#define STRPRT_EQ_ZR(x, str) \
    if (x == 0)              \
        std::cout << str << std::endl;
#define STRPRT(str) std::cout << str << std::endl;
#define RET_EQ_M1(x, ret) \
    if (x == -1)          \
        return ret;
typedef unsigned short U16;
typedef unsigned short port_t;

typedef unsigned int U32;

#define LSTN_MAX 128
#define LSTN_MIN 1
#define DATA_BUFF_SIZE 4096

// exp..al
#define edl +"\n"
#define errmsg "Error: " +

// v2msg define
#define REQUEST_REGISTER 1
#define REQUEST_LOGIN 2
#define REQUEST_FILE 3
#define REQUEST_TOP 4
#define REQUEST_SYSINFO 5

#define MSGTYPE_START_REQUEST 1
#define MSGTYPE_UPDATE_REQUEST 2
#define MSGTYPE_ROOM 3
#define MSGTYPE_PRIVATE 4

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
    unsigned long long header;
    char requestType;
    char username[40];
    char msgType;
    unsigned long long msgLength;
    std::string msg;
    MyMsgV2() : header(0)
    {
        std::string msgStructStr = "DEFAULT_USER";
        memset(username, 0, sizeof username);
        for (int i = 0; i < msgStructStr.size(); i++)
        {
            username[i] = msgStructStr[i];
        }
    };
    MyMsgV2(std::string userName) : header(0)
    {

        memset(username, 0, sizeof username);
        for (int i = 0; i < userName.size(); i++)
        {
            username[i] = userName[i];
        }
    };

    MyMsgV2(const std::string &userName, const std::string &msgToSend) : header(0), msg(msgToSend.c_str())
    {

        memset(username, 0, sizeof username);
        for (int i = 0; i < userName.size(); i++)
        {
            username[i] = userName[i];
        }
        msgLength = msgToSend.size();
        // msg.msg = msgStructStr.c_str();

        // header
        header = sizeof(requestType) + sizeof(username) + sizeof(msgType) + sizeof(msgLength) + msgLength;
    };
    void setMessage(const std::string &msg)
    {
        msgLength = msg.size();
        header = sizeof(requestType) + sizeof(username) + sizeof(msgType) + sizeof(msgLength) + msgLength;
        this->msg = msg;
    }
    std::string getMessage()
    {
        return msg;
    }
} v2msg;

class TcpSocket
{
public:
    TcpSocket();
    ~TcpSocket();
    TcpSocket(int socket);
    int toHost(std::string ip, port_t port);

    // 客户端和服务端通信的时候 客户端需要用这个
    int getConnectFd();
    bool sendMsg(const std::string &msg);
    std::string recvMsg(size_t buffer_size);

    bool SendMsgV2(const v2msg &msgStruct, int cfd = -1);
    std::string recvMsgV2(int cfd, int *code);
    v2msg recvMsgStructV2(int cfd, int *code);

private:
    std::string recv(size_t buffer_size);
    int recvV2(int cfd, char *buffer, size_t buffer_size);
    bool send(const std::string &msg);
    bool sendV2(char *buffer, size_t size, int cfd);

private:
    int connectFd;
};

class serverTCP
{
    // server只负责监听
    // 原本通信部分和客户端部分功能重叠，使用另外一个类
public:
    serverTCP();
    ~serverTCP();
    int stcpListen(port_t port);
    TcpSocket *stcpAccept(struct sockaddr_in *saddr = nullptr);
    int getListenFd();

private:
    int listenFd;
};

#endif
