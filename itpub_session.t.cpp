#include "itpub_session.h"
#include "itpub_config.h"
#include <iostream>
#include <sstream>
#include <string>
#include <boost/filesystem.hpp>
#include "itpub_logger.hpp"
using namespace PointCX;
using namespace boost::filesystem;

void usage(char** argv)
{
    std::cout << argv[0] << " <attachment_url>" << std::endl;
}

bool Trace::isTraceActive = true;

int main(int argc, char** argv)
{

    std::string url = "http://www.itpub.net/attachment.php?aid=OTMyMDU3fGEwYzllMzM1fDE0MTE4NjEwODN8MTQxODE5fDE4OTAzMDQ%3D&fid=61";
    if(argc>1){
        url = argv[1];
    }

    itpub_Config cfg;
    cfg.load("config.cfg");
    std::string username;
    cfg.get("username", username);
    std::string password;
    cfg.get("password", password);

    itpub_Session s;
    int ret = s.login(username, password);
    if(!ret){
        std::cout << "Login Failed!!!" << std::endl; 
        return 0;
    }
    else{
        std::cout << "Login Successful!! " << username << "/" << password << std::endl;
    }
    std::string imgDir;
    cfg.get("captcha.dir", imgDir);
    std::string libDir;
    cfg.get("captcha.lib",libDir);
    std::string bookDir;
    cfg.get("book.dir", bookDir);
    std::string seccode;
    // ret = s.captcha(url.c_str(), 
                       // imgDir.c_str(), libDir.c_str(), seccode); 
    // if(ret==0){
        // std::cout << "Return:" << ret << " SecCode:" << seccode << std::endl;
    // }else if(ret==-1){
        // std::cout << "Can not open downloaded captcha image!!!" << std::endl;
    // }else if(ret==-2){
        // std::cout << "Can not open captcha lib!!!" << std::endl;
    // }
    // typedef itpub_Session::url_list_t::iterator url_iterator_t;
    // itpub_Session::url_list_t attachment_url_list;
    // ret = s.scanThread(url.c_str(), attachment_url_list);
    // int thread_id = s.getThreadId(url);
    // for(url_iterator_t it = attachment_url_list.begin(); it!=attachment_url_list.end(); ++it)
    // {
        // //std::cout << (*it) << std::endl;
        // std::string thread_url = (*it);
        // std::stringstream ss;
        // ss<< bookDir << "/" << thread_id;
        // if(!exists(ss.str()))
            // create_directory(ss.str());
        // s.download(thread_url.c_str(), ss.str().c_str());
    // }

    ret = s.downloadThread(url.c_str(), bookDir.c_str());
    std::cout << "Download Status:" << ret <<std::endl;


    //ret = s.download(url.c_str(), bookDir.c_str());
    //std::cout << "Download Status:" << ret <<std::endl;

}
