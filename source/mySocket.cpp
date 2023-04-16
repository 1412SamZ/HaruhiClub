
#include "mySocket.h"

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
    if(cfd == -1)
    {
        cfd = this -> getConnectFd();
    }
    std::string msgBody = msg.msg;

    char* data = (char*)malloc(
        sizeof(msg.header)
        + sizeof(msg.requestType)
        + sizeof(msg.username)
        + sizeof(msg.msgType)
        + sizeof(msg.msgLength)
        + msg.msgLength
    );
    unsigned long long bigInt = htonl(msg.header);
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
    memcpy(data + dataOffset, msg.msg.c_str(), msg.msgLength);
    dataOffset += msg.msgLength;
    std::string dbg = data;
    sendV2(data, dataOffset, cfd);
    //::send(cfd, data, dataOffset, 0);

    
    free(data);

    return true;
}

bool TcpSocket::sendV2(char* buffer, size_t size, int cfd)
{
    char* bptr = buffer;
    int count = size;
    while(count > 0)
    {
        // 循环发送完所有数据
        int sent = ::send(cfd, bptr, count, 0);
        if(sent == -1)
        {
            std::cout << "Error: sendv2 send = -1" << std::endl;
            return -1;
        }
        else if(sent == 0)
        {
            continue;
        }
        bptr += sent;
        count -= sent;
    }
    
    
    return size;
}

v2msg TcpSocket::recvMsgStructV2(int cfd, int* code = 0)
{
    unsigned long long header = 0;
    v2msg ret;
    if(code == 0)
    {
        int notUsedInt = 0;
        code = &notUsedInt;
    }
    *code = recvV2(cfd, (char*)&header, 8);
    if(*code == -1)
    {
        std::cout << "recv Error " << std::endl;
        return ret;
    }
    if(*code == 0)
    {
        std::cout << "the other side is disconnected" << std::endl;
        std::cout << "this fd will be closed" << std::endl;
        ::close(cfd);
        
        return ret;
    }
    header = ntohl(header);
    //std::cout << "Test: header = " << header << std::endl;
    char* buf = (char*)malloc(header + 1 );
    int rv2ret = recvV2(cfd, buf, header );
    if(*code == -1)
    {
        std::cout << "recv Error " << std::endl;
        free(buf);
        return ret;
    }
    if(rv2ret == 0)
    {
        std::cout << "the other side is disconnected" << std::endl;
        std::cout << "this fd will be closed" << std::endl;
        ::close(connectFd);
        free(buf);
        return ret;
    }
    if(rv2ret != header )
    {
        std::cout << "Error: recvMsgV2 not getting all information" << std::endl;
        free(buf);
        return ret;
    }

    
    ret.header = header;
    ret.requestType = buf[0];
    memcpy(&ret.requestType, buf, sizeof ret.msgType);
    memcpy(&ret.username, buf + 1, sizeof ret.username);
    memcpy(&ret.msgType, buf + 41, sizeof ret.msgType);
    memcpy(&ret.msgLength, buf + 42, sizeof(ret.msgLength));

    char* bufMsg = (char*)malloc(ret.msgLength + 1);

    memcpy(bufMsg, buf + 50, ret.msgLength);
    bufMsg[ret.msgLength] = '\0';
    std::string retMsg(bufMsg);
    ret.msg = retMsg.c_str();
    
    free(buf);
    free(bufMsg);
    

    return ret;
}

std::string TcpSocket::recvMsgV2(int cfd, int* code = 0)
{
    unsigned long long header = 0;
    if(code == 0)
    {
        int notUsedInt = 0;
        code = &notUsedInt;
    }
    *code = recvV2(cfd, (char*)&header, 8);
    if(*code == -1)
    {
        std::cout << "recv Error " << std::endl;
        return "";
    }
    if(*code == 0)
    {
        std::cout << "the other side is disconnected" << std::endl;
        std::cout << "this fd will be closed" << std::endl;
        ::close(cfd);
        
        return "";
    }
    header = ntohl(header);
    //std::cout << "Test: header = " << header << std::endl;
    char* buf = (char*)malloc(header + 1 );
    int rv2ret = recvV2(cfd, buf, header );
    if(*code == -1)
    {
        std::cout << "recv Error " << std::endl;
        free(buf);
        return "";
    }
    if(rv2ret == 0)
    {
        std::cout << "the other side is disconnected" << std::endl;
        std::cout << "this fd will be closed" << std::endl;
        ::close(connectFd);
        free(buf);
        return "";
    }
    if(rv2ret != header )
    {
        std::cout << "Error: recvMsgV2 not getting all information" << std::endl;
        free(buf);
        return "";
    }

    //buf[header - 8] = '\0';
    v2msg ret;
    ret.header = header;
    ret.requestType = buf[0];
    memcpy(&ret.requestType, buf, sizeof ret.msgType);
    memcpy(&ret.username, buf + 1, sizeof ret.username);
    memcpy(&ret.msgType, buf + 41, sizeof ret.msgType);
    memcpy(&ret.msgLength, buf + 42, sizeof(ret.msgLength));

    char* bufMsg = (char*)malloc(ret.msgLength + 1);

    memcpy(bufMsg, buf + 50, ret.msgLength);
    bufMsg[ret.msgLength] = '\0';
    std::string retMsg(bufMsg);
    ret.msg = retMsg.c_str();
    
    free(buf);
    free(bufMsg);
    

    return retMsg;
}

int TcpSocket::recvV2(int cfd, char* buffer ,size_t buffer_size)
{
    int count = buffer_size;
    char* bptr = buffer;

    while(count > 0)
    {
        int recved = ::recv(cfd, bptr, count, 0);
        if(recved == -1)
        {
            std::cout << "Error: recvV2 recved = -1" << std::endl;
            return -1;
        }
        else if(recved == 0)
        {
            // recved==0就是接收完了所有缓冲区信息了，就该退出了
            // 返回值用来判断是不是真的把所有信息接收了
            // 正常应该是buffsize - 0，如果不是说明接收有问题
            // 不像send那样写，不然会一直循环或者阻塞
            // send那边是以缓冲区为准，缓冲区发送完了他就结束了
            // recv是要一直等待的，所以我们这次接收完了就退出了
            // 退出之后看如果没有接到完整信息再去进行别的操作
            return buffer_size - count;
        }

        bptr += recved;
        count -= recved;



    }
    return buffer_size;



}


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
