#include "sqlpp.h"
using namespace std;
Mapi::Mapi(){
    conn = mysql_init(nullptr);
    mysql_set_character_set(conn, "utf8");
}

Mapi::~Mapi(){
    if(conn){
        mysql_close(conn);
    }
    ReleaseResult();
}

bool Mapi::connect(string user, string passwd, string dbName, string ip, unsigned short port)
{
    bool ret;
    if(dbName == "")
    {
        if(mysql_real_connect(conn, ip.c_str(), user.c_str(), passwd.c_str(), NULL, port, NULL, 0))
        {
            return true;
        }    
        else return false;
    }
    else
    {
        if(mysql_real_connect(conn, ip.c_str(), user.c_str(), passwd.c_str(),dbName.c_str(), port, NULL, 0))
        {
            return true;
        }    
        else return false;
    }
}

bool Mapi::update(string sql){
    if(mysql_query(conn, sql.c_str())){
        return false;
    }

    return true;

}

bool Mapi::query(string sql){
    ReleaseResult();
    if(mysql_query(conn, sql.c_str())){
        return false;
    }

    mRes = mysql_store_result(conn);

    return true;

}

bool Mapi::next(){
    if(mRes != nullptr){
        mRow = mysql_fetch_row(mRes);
        if(mRow == NULL)return false;
    }
    else return false;    
    return true;
}

string Mapi::value(int index = 0){
    int rowNumber = mysql_num_fields(mRes);
    if(index >= rowNumber || index < 0){
        return string();
    }
    char* val =  mRow[index];
    unsigned long length = mysql_fetch_lengths(mRes)[index];
    return string(val, length); // avoid '\0'

}




bool Mapi::transaction(){
    return mysql_autocommit(conn, false);
}

bool Mapi::commit(){
    return mysql_commit(conn);
}

bool Mapi::rollback(){
    return mysql_rollback(conn);
}

void Mapi::ReleaseResult(){
    if(mRes){
        mysql_free_result(mRes);
    }
}

