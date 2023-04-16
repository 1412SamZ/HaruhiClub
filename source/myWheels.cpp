
#include "myWheels.h"
using namespace std;

extern std::unordered_map<int, std::string> loginUsers;


vector<string> strsplit(const string& str, char delim )
{
    string s;
    s.append(1, delim);
    std::regex reg(s);
    vector<string> elem(sregex_token_iterator(str.begin(), str.end(), reg, -1), sregex_token_iterator());
    return elem;

}




std::string ExecuteCMD(const char* cmd)
{
    int iRet = -1;
    char result[CMD_RESULT_BUF_SUZE] = {0} ;
    char buf_ps[CMD_RESULT_BUF_SUZE];
    char ps[CMD_RESULT_BUF_SUZE] = {0};
    FILE* ptr;

    strcpy(ps, cmd);

    if((ptr = popen(ps, "r") )!= NULL)
    {
        while(fgets(buf_ps, sizeof(buf_ps), ptr) != NULL)
        {
            strcat(result, buf_ps);
            if(strlen(result) > CMD_RESULT_BUF_SUZE - 1)break;
            memset(buf_ps, 0, CMD_RESULT_BUF_SUZE);
        }
        pclose(ptr);
        ptr = nullptr;
        return result;
    }
    else{
        printf("popen %s failed!\n", ps);
        pclose(ptr);
        return "-1";
    }

}




std::string threadVm(string pid)
{
    string procPath = "/proc/" + pid + "/status";
    FILE* fp1 = fopen(procPath.c_str(), "r");
    if(fp1 == NULL)return "";
    char strV[100];
    char strS[100];
    fgets(strV, 100, fp1);
    
    int k = 21;
    while(k--){
        if(k == 19)fgets(strS, 100, fp1);
        else fgets(strV, 100, fp1);
    }
    // cout << strS << endl;
    int ret = 0;
    sscanf(strV, "VmRSS: %d kB", &ret);
    // cout << ret << endl;
    std::string substrS = strS;
    substrS = substrS.substr(7, substrS.size()-8);
    fclose(fp1);
    return std::to_string(ret) + " " + substrS;
}



std::map<string, std::vector<unsigned long>> mp;
std::string threadCpu(string pid) {
    string procStatPath = "/proc/" + pid + "/stat";
    ifstream procStatFile(procStatPath.c_str());


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



std::string listDir(const string& path )
{
    std::unordered_map<std::string, std::string> retMatch;
    struct dirent* dirProc;
    DIR* dp;
    DIR* pdp;
    dp = opendir(path.c_str());
    if(dp == NULL)perror("Error: proc reading failed!");



    int count = 0;
    while((dirProc = readdir(dp)))
    {
        if(!isPid(dirProc -> d_name))continue;
        std::string pidRet;
        pidRet += threadCpu(dirProc->d_name);
        pidRet += threadVm(dirProc -> d_name);
        

        retMatch[strsplit(pidRet, ' ')[0]] = pidRet;

    }
    closedir(dp);


    auto str = ExecuteCMD("pidstat -d -p ALL");

    auto strvec = strsplit(str);
    int i = 0;
    for(auto item: strvec)
    {
        if(i <= 2)
        {
            i++;
            continue;
        }
        
        auto tempvec = strsplit(item, ' ');
        for(auto it = tempvec.begin(); it != tempvec.end();)
        {
            if(*it == "")
            {
                it = tempvec.erase(it);
            }
            else it++;
        }
        
        if(retMatch.find(tempvec[3]) != retMatch.end())retMatch[tempvec[3]] += " "+ tempvec[4] + " "+ tempvec[5] + " "+ tempvec[6] + " "+ tempvec[8];
        
    }

    std::string ret;
    for(auto retM: retMatch)
    {
        ret += retM.second;
        ret += "\n";
    }
    
    return ret;


}



bool isPid(const char* pidStr)
{
    for(int i = 0; i < strlen(pidStr); i++)
    {
        if(!isdigit(pidStr[i]))return false;
    }
    return true;
}
