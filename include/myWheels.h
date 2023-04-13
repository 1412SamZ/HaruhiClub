#pragma once
#include <stdio.h>
#include<string.h>

#define CMD_RESULT_BUF_SUZE 8192



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
        return "";
    }

}




