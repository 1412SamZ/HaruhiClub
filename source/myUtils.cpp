#include"myUtils.h"


// std::unordered_map<int, std::string> loginUsers;
extern std::unordered_map<int, std::string> loginUsers;
void loginCheck(std::string username, std::string password, int cfd)
{
    Mapi mc;
    bool connected = mc.connect("root", "Zhanggy123", "", "localhost", 0);
    cout << "SQL SERVER CONNECTION STATUS: " << connected << endl;
    string uname, upswd;
    
    mc.update(
        "use BGManagement"
    );


    //cmd = "show databases";
    string cmd = "select username, password from login where username = 'moon'";

    mc.query(cmd);    
    while(mc.next()){
        uname = mc.value(0);
        
        upswd = mc.value(1);
        
    }


    if(uname == username && upswd == password)
    {
        loginUsers[cfd] = username;
        cout << "User: " << username <<" login successed" << endl;
    }
    else cout << "User: " << username <<" login failed" << endl;

    cout << "thread end" << endl;
    return;

}