#include <thread>
#include <unistd.h>
#include "myWheels.h"
#include "mySocket.h"
using namespace std;

extern bool pidThreadStop;

void pidThread(TcpSocket* stcp, int curfd);

void fileThread(TcpSocket* stcp, int curfd);