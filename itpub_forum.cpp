#include "itpub_session.h"
#include "itpub_config.h"
#include <iostream>
#include <sstream>
#include <string>
#include <boost/filesystem.hpp>
#include "itpub_logger.hpp"
using namespace PointCX;
using namespace boost::filesystem;
using namespace std;
#ifdef DEBUG
bool Trace::isTraceActive = true;
#else
bool Trace::isTraceActive = false;
#endif //DEBUG
int main(int argc, char** argv)
{
    Trace log("itpub_forum");
    std::string forum_url;
    if(argc>1){
        forum_url = argv[1];
    }else{
        std::cout << argv[0] << " <forum_url>" << std::endl;
    }
    //-----------------------------------------------------------------
    // Login 
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
    //-----------------------------------------------------------------
    //  scan forum
    std::string bookDir;
    cfg.get("book.dir", bookDir);
    typedef itpub_Session::url_list_t::iterator url_iterator_t;
    itpub_Session::url_list_t thread_url_list;
    s.scanForum(forum_url.c_str(), thread_url_list);
    //-----------------------------------------------------------------
    // Download
    for(url_iterator_t it=thread_url_list.begin(); it!=thread_url_list.end(); ++it)
    {
        std::string thread_url = *it;
        s.downloadThread(thread_url.c_str(), bookDir.c_str());
    }
    return 0;
}
