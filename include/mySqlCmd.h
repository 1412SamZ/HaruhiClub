
#pragma once

#include<string>

using namespace std;


class SqlCmd
{
    public:
    string myCmd;

    


    void mySelect(string myCol);
    void myFrom(string myTable);
    void myWhere(string myCondi);

    void myOrderby(string myOrderCol);

    private:
    void _ccat();


    
};