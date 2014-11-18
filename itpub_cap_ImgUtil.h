#ifndef ITPUB_CAP_IMAGE_UTIL_HEAD
#define ITPUB_CAP_IMAGE_UTIL_HEAD

#include <vector>
#include <string>
#include <utility> // std::pair
#include <cstdio>
#include <sstream>

class rgb_t
{
    public:
        unsigned r;
        unsigned g;
        unsigned b;
        unsigned int count;
};

class captcha_word
{
    public:
        typedef std::vector<int> row_t;
        typedef std::vector<row_t> matrix_t;

        matrix_t matrix;

        int width;
        int height;

        int offset;

        captcha_word():width(0), height(0)
        {}

        void string(std::string& result)
        {
            std::stringstream ss;

           for(int i=0; i<height; i++)
           {
               const row_t &row = matrix[i];
               for(int j=0;j<width; j++)
               {
                  ss<< row[j]; 
               }
           } 
           result.append(ss.str());
        }

        void print() const{
            for(int i=0; i< height; i++)
            {
                const row_t &row = matrix[i];
                for(int j=0; j<width; j++)
                {
                    printf("%d ", row[j]);
                }
                printf("\n");
            }
        }
};

inline bool operator<(const captcha_word& p1, const captcha_word& p2)
{
    return p1.offset < p2.offset;
}

inline int word_compare(const captcha_word& p1, const captcha_word& p2)
{
    return p1.offset > p2.offset;
}


class itpub_cap_PngPixelIter;
class itpub_cap_ImgUtil_i;
class itpub_cap_ImgUtil
{
    public:
        typedef std::vector<rgb_t> rgb_list;
        typedef rgb_list::iterator rgb_list_iter;


    private:
        itpub_cap_ImgUtil_i *d_this;
        itpub_cap_ImgUtil& operator=(const itpub_cap_ImgUtil&);     // Not Implemented
        itpub_cap_ImgUtil(const itpub_cap_ImgUtil&);                // Not Implemented
    public:
        itpub_cap_ImgUtil();
        ~itpub_cap_ImgUtil();

        //Methods

        int sort_by_pixel(itpub_cap_PngPixelIter& png_iter, rgb_list& );
        int top_colors(itpub_cap_PngPixelIter& png_iter, rgb_list& );
        int split_by_color(itpub_cap_PngPixelIter& png_iter, const rgb_t& rgb, captcha_word& word);
};

#endif // ITPUB_CAP_IMAGE_UTIL_HEAD
