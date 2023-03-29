
// #pragma once
// #ifndef easy_socket_h
// #define easy_socket_h

// /*下面都是平台适配*/
// #if __linux__
// #include <unistd.h>  //close 和 shutdown
// #include <sys/types.h>
// #include <sys/socket.h>
// #include <netinet/in.h>
// #include <arpa/inet.h> //inet_ntop
// #include <string.h>

// #define SOCKET_ERROR (-1)
// #define CLOSE_SOCKET(x) ::close(x);
// #define SET_IP(socket_addr,str_ip) \
// socket_addr.sin_addr.s_addr = inet_addr(str_ip.c_str());

// #elif _WIN32
// //win64下也有_win32宏为1
// #include <winsock.h>
// #pragma comment(lib,"ws2_32.lib")

// #define CLOSE_SOCKET(x) closesocket(x);
// #define SET_IP(socket_addr,str_ip) \
// _socket_addr.sin_addr.S_un.S_addr = inet_addr(str_ip.c_str());

// class win_socket_dll {
// public:
// 	win_socket_dll() {
// 		uint16_t w_req = MAKEWORD(2, 2);//版本号
// 		WSADATA wsadata;
// 		if (WSAStartup(w_req, &wsadata) != 0) exit(1);
// 		if (LOBYTE(wsadata.wVersion) != 2 || HIBYTE(wsadata.wHighVersion) != 2) {
// 			WSACleanup();
// 			exit(2);
// 		}
// 	}
// 	~win_socket_dll() {
// 		WSACleanup();
// 	}
// };
// static win_socket_dll dll;

// #if _WIN64
// #pragma warning (disable:4267)
// #pragma warning (disable:4244)
// #endif

// #endif

// #include <string>
// #include <cassert>

// namespace easySocket {
// 	namespace ipv4 {
// 		class socket_t {
// 			class address_t {
// 				sockaddr_in _socket_addr;

// 			public:
// 				address_t() { memset(&_socket_addr, 0, sizeof(_socket_addr)); };

// 				address_t(std::string const& str_ip, uint16_t port) {
// 					set(str_ip, port);
// 				};

// 				void set(std::string const& str_ip, uint16_t port) {
// 					_socket_addr.sin_family = AF_INET;  //TCP ipv4
// 					SET_IP(_socket_addr, str_ip);  //点分十进制ip转32位
// 					_socket_addr.sin_port = htons(port); //转成网络字节序
// 				}

// 				sockaddr const* const socket_addr()const {
// 					return (sockaddr*)(&_socket_addr);
// 				}

// 				size_t size()const {
// 					return sizeof(sockaddr);
// 				}
// 			};
// 			address_t _addr; // ip和端口
// 			uint32_t _socket_fd; // socket描述字
// 		public:
// 			socket_t() { _socket_fd = socket(AF_INET, SOCK_STREAM, 0); };
// 			socket_t(std::string const& str_ip, uint16_t port) :_addr(str_ip, port) {
// 				//最后一个参数填0时自动选择默认的，也可以指定为IPPROTO_TCP。
// 				_socket_fd = socket(AF_INET, SOCK_STREAM, 0);
// 				assert(::bind(_socket_fd, _addr.socket_addr(), _addr.size()) != SOCKET_ERROR);
// 			};
// 			~socket_t() { this->close(); };

// 			bool bind(std::string const& str_ip, uint16_t port) {
// 				_addr.set(str_ip, port);
// 				return ::bind(_socket_fd, _addr.socket_addr(), _addr.size()) == SOCKET_ERROR ? false : true;
// 			}

// 			bool connect(std::string const& str_ip, uint16_t port) {
// 				address_t server_addr(str_ip, port);
// 				return ::connect(_socket_fd, server_addr.socket_addr(), server_addr.size()) != SOCKET_ERROR;
// 			}

// 			bool send(std::string const& msg) {
// 				return ::send(_socket_fd, msg.c_str(), msg.size() + 1, 0) >= 0;
// 			}

// 			bool recv(std::string& msg, size_t buffer_size = 1024 * 8) {
// 				char* p_buffer = new char[buffer_size]; //8KB
// 				if (::recv(_socket_fd, p_buffer, buffer_size, 0) <= 0) {
// 					delete[] p_buffer;
// 					return false;
// 				}
// 				msg = p_buffer;
// 				delete[] p_buffer;
// 				return true;
// 			}

// 			bool sendmsg(std::string const& longmsg) {
// 				//先告知对面准备多少空间接收
// 				if (send(std::to_string(longmsg.size() + 1)) == false)return false;
// 				return send(longmsg);
// 			}

// 			bool recvmsg(std::string& longmsg) {
// 				//先看一下需要开多大的缓冲区
// 				std::string str_length;
// 				if (recv(str_length) == false)return false;
// 				size_t length = std::stoi(str_length);
// 				return recv(longmsg, length + 1);  //这里玄学的加一就好了......
// 			}

// 			bool listen(size_t num = 10) {
// 				return ::listen(_socket_fd, num) >= 0;
// 			}

// 			bool accept(socket_t& socket) {
// 				//后面两个参数可以获得客户端的信息。这里置空不去管。
// 				socket._socket_fd = ::accept(_socket_fd, NULL, NULL);
// 				return socket._socket_fd != SOCKET_ERROR;
// 			}

// 			void close() {
// 				CLOSE_SOCKET(_socket_fd);
// 			}
// 		};
// 	}
// }
// #endif //easy_socket_h