#include"itpub_http.h"
#include "itpub_config.h"
#include <iostream>
#include <string>

using namespace PointCX;

typedef itpub_http_Client<itpub_http_OutputStrPolicy> login_client;
typedef itpub_http_Client<itpub_http_OutputFilePolicy, itpub_http_GetPolicy> http_get_client;

int main(int argc, char** argv)
{
    itpub_Config cfg;
    cfg.load("/itpub/config.cfg");
    login_client lc;
    lc.setCookie("itpub_cookie");
    std::string res;
    lc.setOutput(&res);
    //lc.setOutput("/itpub/test.txt");
    itpub_http_PostPolicy::ValueType name_value_list;
    std::string username;
    cfg.get("username", username);
    std::string password;
    cfg.get("password", password);
    name_value_list.push_back(std::make_pair("username", username));
    name_value_list.push_back(std::make_pair("password", password));
    lc.form_add(name_value_list);
    int ret = lc.request("http://www.itpub.net/member.php?mod=logging&action=login&loginsubmit=yes&infloat=yes&lssubmit=yes&inajax=1");

    std::cout << ret << std::endl;
    std::cout << res << std::endl;


    http_get_client hgc;
    hgc.setCookie("itpub_cookie");
    //hgc.setOutput(res);
    hgc.setOutput("/itpub/test.txt");
    ret = hgc.request("http://www.itpub.net/attachment.php?aid=OTMyMDU3fGEwYzllMzM1fDE0MTE4NjEwODN8MTQxODE5fDE4OTAzMDQ%3D&fid=61");

    std::cout << ret << std::endl;
    std::cout << res << std::endl;
}
