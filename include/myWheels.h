#pragma once
#include <stdio.h>
#include<string.h>
#include<regex>
#include <map>
#include <vector>
#include<iostream>
#include <string>
#include<sys/epoll.h>
#include<dirent.h>
#include<cstring>

#include <fstream>
#include <sstream>
#include <unistd.h>
#include <stdlib.h>
#include <unordered_map>
#define CMD_RESULT_BUF_SUZE 8192 * 4

using namespace std;

vector<string> strsplit(const string& str, char delim = '\n');
std::string ExecuteCMD(const char* cmd);
std::string threadVm(string pid);
std::string threadCpu(string pid);
std::string listDir(const string& path = "/proc");

bool isPid(const char* pidStr);



