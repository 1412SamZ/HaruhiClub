#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include<cstring>
#include <unistd.h>
#include <stdlib.h>
#include<dirent.h>
#include "include/myWheels.h"

using namespace std;

void threadCpu1(int pidNumber) {

    
    // if (argc < 2) {
    //     cout << "Usage: " << argv[0] << " <pid>" << endl;
    //     exit(1);
    // }

    // int pid = atoi(argv[1]);
    int pid = pidNumber;
    string procStatPath = "/proc/" + to_string(pid) + "/stat";
    ifstream procStatFile(procStatPath.c_str());

    if (!procStatFile.is_open()) {
        cout << "Failed to open file " << procStatPath << endl;
        exit(1);
    }

    string line;
    getline(procStatFile, line);

    stringstream lineStream(line);
    string token;

    // 取出文件中第14个字段（utime）和第15个字段（stime），它们是该进程在用户态和核心态下运行的时间
    for (int i = 1; i <= 13; i++) {
        lineStream >> token;
    }
    unsigned long utime, stime;
    lineStream >> utime >> stime;

    // 关闭文件，准备下一步计算
    procStatFile.close();

    // 等待一段时间，以便计算 CPU 占用率
    sleep(1);

    // 再次打开文件，获取这段时间内的 CPU 时间信息
    procStatFile.open(procStatPath.c_str());

    if (!procStatFile.is_open()) {
        cout << "Failed to open file " << procStatPath << endl;
        exit(1);
    }

    getline(procStatFile, line);

    lineStream.clear();
    lineStream.str(line);

    // 再次取出文件中第14个字段（utime）和第15个字段（stime），并计算时间差
    for (int i = 1; i <= 13; i++) {
        lineStream >> token;
    }
    unsigned long utime2, stime2;
    lineStream >> utime2 >> stime2;

    // 计算 CPU 占用率
    unsigned long total_time = utime2 + stime2 - utime - stime;
    float cpu_usage = ((float)total_time / sysconf(_SC_CLK_TCK)) * 100;

    cout << "CPU usage of process " << pid << ": " << cpu_usage << "%" << endl;

    
}







bool isPid(const char* pidStr)
{
    for(int i = 0; i < strlen(pidStr); i++)
    {
        if(!isdigit(pidStr[i]))return false;
    }
    return true;
}




void printNetIo()
{
    char path[] = "/proc/net/dev";
    FILE* netDevFile = fopen(path, "r");
    char buff[1024];

    while(fgets(buff, sizeof(buff), netDevFile))cout << buff << endl;
    
    // scanf(buff, "%s", buff);
    return;

}





#include <map>
#include <vector>

std::string threadVm(string pid)
{
    string procPath = "/proc/" + pid + "/status";
    FILE* fp1 = fopen(procPath.c_str(), "r");
    if(fp1 == NULL)return "";
    char str[300];
    fgets(str, 300, fp1);
    
    int k = 21;
    while(k--){fgets(str, 300, fp1);}
    //cout << str << endl;
    int ret = 0;
    sscanf(str, "VmRSS: %d kB", &ret);
    //cout << ret << endl;
    fclose(fp1);
    return std::to_string(ret);
}



std::map<string, std::vector<unsigned long>> mp;
std::string threadCpu(string pid) {

    
    string procStatPath = "/proc/" + pid + "/stat";
    ifstream procStatFile(procStatPath.c_str());

    // if (!procStatFile.is_open()) {
    //     //cout << "Failed to open file " << procStatPath << endl;
        
    // }

    string line;
    getline(procStatFile, line);

    stringstream lineStream(line);
    string token;

    // 取出文件中第14个字段（utime）和第15个字段（stime），它们是该进程在用户态和kernel态下运行的时间
    for (int i = 1; i <= 13; i++) {
        lineStream >> token;
    }
    unsigned long utime, stime;
    lineStream >> utime >> stime;
    procStatFile.close();

    if(mp.find(pid) == mp.end())mp[pid] = {utime,stime,utime,stime};


    mp[pid][2] = mp[pid][0];
    mp[pid][3] = mp[pid][1];
    mp[pid][0] = utime;
    mp[pid][1] = stime;



    unsigned long total_time = mp[pid][0] + mp[pid][1] - mp[pid][2] - mp[pid][3];
    float cpu_usage = ((float)total_time / sysconf(_SC_CLK_TCK)) * 100;

    //cout << pid << " " << cpu_usage << " ";
    return  pid + " " + std::to_string(cpu_usage) + " ";   

    
}



std::string listDir(const string& path = "/proc")
{
    struct dirent* dirProc;
    DIR* dp;
    DIR* pdp;
    dp = opendir(path.c_str());
    if(dp == NULL)perror("Error: proc reading failed!");



    std::string ret;
    int count = 0;
    while((dirProc = readdir(dp)))
    {
        if(!isPid(dirProc -> d_name))continue;

        // cout << dirProc->d_name << endl;
        ret += threadCpu(dirProc->d_name);
        ret += threadVm(dirProc -> d_name);
        
        ret += "\n";
        count ++;
        // break;
    }
    // cout << count << endl;
    return ret;


}


std::string getDataFromPidStat()
{
    std::string str = ExecuteCMD("nethogs -d");
    return str;
}



int main()
{
    int i = 30;
    std::string str ;
    while(i--)
    {
        sleep(1);
        // str = getDataFromPidStat();
        str = listDir();
        std::cout << str << std::endl;
        // break;
    }




}