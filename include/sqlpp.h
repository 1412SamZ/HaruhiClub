#pragma once
#include<mysql/mysql.h>
#include<string>


using namespace std;
class Mapi{
    MYSQL* conn = nullptr;
    MYSQL_RES* mRes = nullptr;
    MYSQL_ROW mRow = nullptr;

    void ReleaseResult();

    public:
    
    Mapi();
    
    ~Mapi();

    bool connect(string user, string passwd, string dbName, string ip, unsigned short port);

    bool update(string sql);

    bool query(string sql);

    bool next();

    string value(int index);
        
    bool transaction();

    bool commit();

    bool rollback();





};


