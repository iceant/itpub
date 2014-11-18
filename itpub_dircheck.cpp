#include "itpub_config.h"
#include <iostream>
#include <vector>
#include <string>
#include <boost/filesystem.hpp>

using namespace PointCX;

inline void split(const std::string& s, const std::string& delim, std::vector< std::string >& ret)
{
    size_t last = 0;
    size_t index=s.find_first_of(delim,last);
    while (index!=std::string::npos)
    {
        ret.push_back(s.substr(last,index-last));
        last=index+1;
        index=s.find_first_of(delim,last);
    }
    if (index-last>0)
    {
        ret.push_back(s.substr(last,index-last));
    }
}

int main(int argc, char** argv)
{
    itpub_Config cfg;
    std::string dir;
    cfg.load("config.cfg");
    cfg.get("book.dir", dir);
    std::string threads_done_str;
    cfg.get("threads.done", threads_done_str);
    std::vector<std::string> threads_done_list;
    boost::filesystem::path path(dir);
    split(threads_done_str, ",", threads_done_list);
    int nIdx=0;
    for(std::vector<std::string>::const_iterator it=threads_done_list.begin();
            it!=threads_done_list.end(); ++it)
    {
        boost::filesystem::path thread_path = path/(*it);
        if(!boost::filesystem::exists(thread_path))
        {
            std::cout << ++nIdx << " "<< thread_path << std::endl;
        }
    }
}
