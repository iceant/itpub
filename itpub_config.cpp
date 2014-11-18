#include "itpub_config.h"
#include <vector>
#include <utility> //pair
#include <iostream>
#include <fstream>

namespace PointCX
{
    //=============================================================================
    // itpub_Config_i
    //=============================================================================
    class itpub_Config_i
    {
        friend class itpub_Config;

        public:
        typedef std::pair<std::string, std::string> value_t;
        typedef std::vector<value_t> config_t;

        bool isExist(const std::string& key) const
        {
            for(config_t::const_iterator it=d_config.begin(); it!=d_config.end(); ++it)
            {
                if((*it).first.compare(key)==0)
                {
                    return true;
                }
            }
            return false;
        }

        bool remove(const std::string& key)
        {
            for(config_t::iterator it=d_config.begin(); it!=d_config.end(); ++it)
            {
                if((*it).first.compare(key)==0)
                {
                    d_config.erase(it);
                    return true;
                }
            }
            return false;
            
        }

        config_t d_config;
    };

    //=============================================================================
    // itpub_Config
    //=============================================================================
    itpub_Config::itpub_Config():d_this(new itpub_Config_i){}

    itpub_Config::~itpub_Config()
    {
        delete d_this;
    } 

    int itpub_Config::set(const std::string& key, const std::string& value)
    {
        enum{OK=0, ERR_KEY_EXIST=-1};
        // if(d_this->isExist(key))
        // {
            // return ERR_KEY_EXIST; 
        // }
        d_this->remove(key);
        d_this->d_config.push_back(std::make_pair(key, value));
        return OK;
    }

    int itpub_Config::get(const std::string& key, std::string& ret) const
    {
        enum{OK=0, ERR_NONE_EXIST=-1};
		
        for(itpub_Config_i::config_t::iterator it = d_this->d_config.begin(); it!=d_this->d_config.end(); ++it)
        {
			//std::cout << "Key:" << key << " VS [" << (*it).first << "]" << std::endl;
            if(key.compare((*it).first)==0)
            {
                ret = (*it).second;
                return OK;
            }
        }
        return ERR_NONE_EXIST;
    }


    int itpub_Config::save(const std::string& file)
    {
        std::ofstream ofs;
        ofs.open(file.c_str(), std::ios::out|std::ios::trunc);
        if(!ofs.is_open())
        {
            return -1;
        }
        ofs << *this;
        ofs.flush();
        ofs.close();
        return 0;
    }


    int itpub_Config::load(const std::string& file)
    {
        d_this->d_config.clear();
        enum{OK=0, ERR_FILE_DONT_EXIST=-1};
        std::ifstream ifs(file.c_str());
        if(!ifs)
            return ERR_FILE_DONT_EXIST;
        ifs >> *this;
        ifs.close();
        return OK;
    } 

    int itpub_Config::keys(std::vector<std::string>& ret) const
    {
        for(itpub_Config_i::config_t::const_iterator it=d_this->d_config.begin();
                it!=d_this->d_config.end();
                ++it){
            ret.push_back((*it).first);
        }
        return 0;
    }

    //=============================================================================
    // IOSTREAM
    //=============================================================================
    std::ostream& operator<<(std::ostream & os, const itpub_Config &item)
    {
        for(PointCX::itpub_Config_i::config_t::iterator it=item.d_this->d_config.begin();
                it!=item.d_this->d_config.end();
                ++it){
            os << (*it).first << "=" << (*it).second << std::endl;
        }
        return os;
    }

    std::istream& operator>>(std::istream& is, itpub_Config &item)
    {
        while(!is.eof()) {
            std::string key;
            std::string value;
            std::string line;
            std::getline(is, line);
            std::size_t pos_start = line.find("=");
            if(pos_start!=std::string::npos)
            {
                key = line.substr(0, pos_start);
                value = line.substr(pos_start+1, line.size());
                if(item.d_this->isExist(key)) continue; // ignore the exist key
                item.d_this->d_config.push_back(std::make_pair(key, value));
            }
        }
        return is;
    }


}//End Namespace PointCX
