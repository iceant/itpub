#ifndef ITPUB_CAP_STUDY_HEADER
#define ITPUB_CAP_STUDY_HEADER

#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <stdexcept>
#include "simil.h"
#include "itpub_cap_ImgUtil.h"
#include "itpub_cap_PngPixelIter.h"

struct record_t
{
    std::string token;
    char font;

    record_t():token(""), font(0)
    {}

    record_t(const record_t& that){
        token = that.token;
        font = that.font;
    }

    record_t& operator=(const record_t& rht){
        token = rht.token;
        font = rht.font;
        return *this;
    }

    friend std::ostream& operator<<(std::ostream&, const record_t&);
    friend std::istream& operator>>(std::istream&, record_t& rec);
};

std::ostream& operator<< (std::ostream& os, const record_t& record)
{
    os << record.font;
    os << std::endl;
    os << record.token;
    os << std::endl;
    return os;
}

std::istream& operator>>(std::istream& is, record_t& rec)
{
    is >> rec.font;
    is >> rec.token;
    return is;
}

inline bool is_exist(const std::string& token, const std::vector<record_t>& records)
{
    for(std::vector<record_t>::const_iterator itr=records.begin(); itr!=records.end(); ++itr)
    {
        if(token.compare((*itr).token)==0)
            return true;
    }
    return false;
}

inline bool find_font(const std::string& token, const std::vector<record_t>& records, char& c)
{
    for(std::vector<record_t>::const_iterator itr = records.begin(); itr!=records.end(); ++itr)
    {
        if(token.compare((*itr).token)==0)
        {
            c = (*itr).font;
#ifdef DEBUG
            std::cout << c << "'s token in lib:" << token << std::endl;
#endif //DEBUG
            return true;
        } 
    }
    return false;
}

inline bool read_lib(const std::string& file, std::vector<record_t>& records)
{
    std::ifstream ifile(file);
    if(!ifile) return false;
    while(!ifile.eof())
    {
        record_t rec;
        ifile>>rec.font;
        ifile>>rec.token;
        //std::cout << "Read:" << rec.font << " Token:" << rec.token << std::endl;
        records.push_back(rec);
    }
    return true;
}

inline float find_similar(const std::string& token, const std::vector<record_t>& records, char& ret)
{
    float max_simil=0;
    for(std::vector<record_t>::const_iterator itr = records.begin(); itr!=records.end(); ++itr)
    {
        float s = simil(token.c_str(), (*itr).token.c_str());
		if(s==1L){
			ret = (*itr).font;
			#ifdef DEBUG
			std::cout <<"=>" << ret << " " << (*itr).token << std::endl;
			#endif
			return s;
		}else
        if(s>max_simil)
        {
            max_simil = s;
            ret = (*itr).font;
			#ifdef DEBUG
			std::cout << "->" << s << " " << ret << " " <<(*itr).token << std::endl;
			#endif
        }
    }
    return max_simil;
}


inline bool read_img(const std::string& file, std::vector<record_t>& records, std::vector<char>& ret)
{
	//std::cout << file << "  " << records.size() << std::endl;
	std::ifstream ifile(file);
	if(!ifile){
		return false;
	}
	itpub_cap_PngPixelIter png_iter(file.c_str());
	itpub_cap_ImgUtil iu;
	itpub_cap_ImgUtil::rgb_list rgbs;
	iu.top_colors(png_iter, rgbs);

	captcha_word w1;
	captcha_word w2;
	captcha_word w3;
	captcha_word w4;

	iu.split_by_color(png_iter, rgbs[0], w1);
	iu.split_by_color(png_iter, rgbs[1], w2);
	iu.split_by_color(png_iter, rgbs[2], w3);
	iu.split_by_color(png_iter, rgbs[3], w4);
	std::vector<captcha_word> d_words;

	d_words.push_back(w1);
	d_words.push_back(w2);
	d_words.push_back(w3);
	d_words.push_back(w4);
	std::sort(d_words.begin(), d_words.end());
	//std::cout << "Read img:" << file << std::endl;
	for(int i=0; i<4; i++)
    {
		std::string token;
		d_words[i].string(token);
		char c='0';
		
		if(find_font(token, records, c)){
			ret.push_back(c);
		}else{
			find_similar(token, records, c); 
			std::cout << "Find Similar:" << c << std::endl;
			ret.push_back(c);
		}
	}
	return true;
}

#endif //ITPUB_CAP_STUDY_HEADER
