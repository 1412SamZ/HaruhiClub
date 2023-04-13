#include<iostream>
#include<unistd.h>
#include<dirent.h>
#include<string.h>
#include <fstream>

using namespace std;



#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <unistd.h>

int main(int argc, char* argv[]) {
    std::string proc_stat_path = "/proc/" + std::string(argv[1]) + "/stat";
    std::ifstream proc_stat_file(proc_stat_path);

    if (!proc_stat_file.is_open()) {
        std::cerr << "Failed to open file: " << proc_stat_path << std::endl;
        return 1;
    }

    std::string line;
    std::getline(proc_stat_file, line);
    std::istringstream iss(line);

    // skip first 13 fields
    for (int i = 0; i < 13; ++i) {
        std::string field;
        iss >> field;
    }

    // get utime and stime
    unsigned long utime, stime;
    iss >> utime >> stime;

    // get total time
    unsigned long total_time = utime + stime;

    // get start time
    unsigned long start_time;
    iss >> start_time;

    // calculate time interval
    unsigned long hertz = sysconf(_SC_CLK_TCK);
    unsigned long uptime_seconds;
    std::ifstream uptime_file("/proc/uptime");
    uptime_file >> uptime_seconds;
    unsigned long interval = uptime_seconds - (start_time / hertz);

    // calculate CPU usage percentage
    double cpu_usage_percentage = 100 * ((double)total_time / hertz) / interval;

    std::cout << "Process " << argv[1] << " CPU usage: " << cpu_usage_percentage << "%" << std::endl;

    return 0;
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




void listDir(const string& path)
{
    struct dirent* dirProc;
    DIR* dp;
    DIR* pdp;
    dp = opendir(path.c_str());
    if(dp == NULL)perror("Error: proc reading failed!");

    while((dirProc = readdir(dp)))
    {
        if(!isPid(dirProc -> d_name))continue;

        cout << dirProc->d_name << endl;
        string tempPidPath = path + "/" + dirProc->d_name;
        pdp = opendir(tempPidPath.c_str());
        while(dirProc = readdir(pdp))
        {
            cout << dirProc->d_name << endl;
        }
        // break;
        if(pdp == NULL)perror("Error: proc sub files reading failed");
    }


}







