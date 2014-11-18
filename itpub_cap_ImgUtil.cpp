#include "itpub_cap_ImgUtil.h"
#include "itpub_cap_PngPixelIter.h"
#include <png.h>
#include <algorithm>
#include <cmath>

//=============================================================================
// itpub_cap_ImgUtil_i
//=============================================================================
class itpub_cap_ImgUtil_i
{};

//=============================================================================
// itpub_cap_ImgUtil 
//=============================================================================

itpub_cap_ImgUtil::itpub_cap_ImgUtil():d_this(new itpub_cap_ImgUtil_i)
{}

itpub_cap_ImgUtil::~itpub_cap_ImgUtil()
{
    if(d_this!=NULL)
    {
        delete d_this;
    }
}

bool operator<(rgb_t& p1, rgb_t& p2)
{
    return p1.count < p2.count;
}

int rgb_compare(const rgb_t& p1, const rgb_t& p2)
{
    return p1.count > p2.count;
}

float color_distance(itpub_cap_PngPixelIter& png_iter, int x1, int y1, int x2, int y2)
{
    //printf("x1:%d, y1:%d, x2:%d, y2:%d\n\r", x1, y1, x2, y2);

    if( (x2<0) || (x2 >= png_iter.width()) || (y2 <0) || (y2 >= png_iter.height()) )
        return -1;

    png_iter.set(x1, y1);
    int r1 = png_iter.r();
    int g1 = png_iter.g();
    int b1 = png_iter.b();

    png_iter.set(x2, y2);
    int r2 = png_iter.r();
    int g2 = png_iter.g();
    int b2 = png_iter.b();

    float d1 = pow(abs(r1-r2),2);
    float d2 = pow(abs(g1-g2),2);
    float d3 = pow(abs(b1-b2),2);

    return sqrt(d1 + d2 + d3);
}

bool is_interfere(itpub_cap_PngPixelIter& png_iter, int x, int y)
{
    int d_x, d_y;
    d_x = png_iter.x();
    d_y = png_iter.y();

    float top_left = color_distance(png_iter, x, y, x-1, y-1);
    float top_center = color_distance(png_iter, x, y, x, y-1);
    float top_right = color_distance(png_iter, x, y, x-1, y);
    float left = color_distance(png_iter, x, y, x-1, y);
    float right = color_distance(png_iter, x, y, x+1, y);
    float bottom_left = color_distance(png_iter, x, y, x-1, y+1);
    float bottom_center = color_distance(png_iter, x, y, x, y+1);
    float bottom_right = color_distance(png_iter, x, y, x+1, y+1);

    //printf("[x:%d, y:%d]=t_l:%f, t_c:%f, t_r:%f, le:%f, ri:%f, b_l:%f, b_c:%f, b_r:%f\n\r", x, y, top_left, top_center, top_right, left, right, bottom_left, bottom_center, bottom_right);

    int count = 8;
    float distance = 0.0L;

    if(top_left==-1 || top_left > distance) count--;
    if(top_center==-1 || top_center > distance) count--;
    if(top_right==-1 || top_right > distance) count--;
    if(left==-1 || left > distance) count--;
    if(right==-1 || right > distance) count--;
    if(bottom_left==-1 || bottom_left > distance) count--;
    if(bottom_center==-1 || bottom_center > distance) count--;
    if(bottom_right==-1 || bottom_right > distance) count--;

    png_iter.set(d_x, d_y);

    if(count < 3) 
        return true;
    return false;
}

int itpub_cap_ImgUtil::sort_by_pixel(itpub_cap_PngPixelIter& png_iter, itpub_cap_ImgUtil::rgb_list& blist)
{

    for(png_iter.reset(); png_iter.next(); ++png_iter)
    {

        int d_x = png_iter.x();
        int d_y = png_iter.y();

        if(is_interfere(png_iter, png_iter.x(), png_iter.y())){
            png_iter.set(d_x, d_y);
            continue;
        }
        png_iter.set(d_x, d_y);

        rgb_t d_rgb;
        d_rgb.r = png_iter.r();
        d_rgb.g = png_iter.g();
        d_rgb.b = png_iter.b();
        d_rgb.count = 1;

        int found = 0;


        for(rgb_list_iter it = blist.begin(); it!=blist.end(); it++)
        {
            rgb_t* t_rgb = &(*it);
            if(t_rgb->r == d_rgb.r && t_rgb->g ==d_rgb.g && t_rgb->b == d_rgb.b)
            {
                t_rgb->count++;
                found = 1; 
                break;
            }
        }

        if(found==0)
        {
            blist.push_back(d_rgb);    
        }
    }
    std::sort(blist.begin(), blist.end(), rgb_compare);
    return 0;
}

inline bool is_black(rgb_t& rgb){
    return rgb.r==0 && rgb.g==0 && rgb.b==0;
}

inline void check_left(itpub_cap_PngPixelIter& png_iter, itpub_cap_ImgUtil::rgb_list& list)
{
    const int d_x = png_iter.x();
    const int d_y = png_iter.y();
    png_iter.set(d_x-1, d_y);
    rgb_t d_left;
    d_left.r = png_iter.r();
    d_left.g = png_iter.g();
    d_left.b = png_iter.b();
    d_left.count=1;

    if(is_black(d_left))
    {
        png_iter.set(d_x, d_y);
        return;
    }

    int found = 0;

    for(itpub_cap_ImgUtil::rgb_list_iter it = list.begin(); it!=list.end(); it++)
    {
        rgb_t* t_rgb = &(*it);
        if(t_rgb->r == d_left.r && t_rgb->g ==d_left.g && t_rgb->b == d_left.b)
        {
            t_rgb->count++;
            found = 1; 
            break;
        }
    }

    if(found==0)
    {
        list.push_back(d_left);    
    }
    png_iter.set(d_x, d_y);
}

inline void check_top(itpub_cap_PngPixelIter& png_iter, itpub_cap_ImgUtil::rgb_list& list)
{
    const int d_x = png_iter.x();
    const int d_y = png_iter.y();
    png_iter.set(d_x, d_y-1);
    rgb_t d_left;
    d_left.r = png_iter.r();
    d_left.g = png_iter.g();
    d_left.b = png_iter.b();
    d_left.count=1;

    if(is_black(d_left))
    {
        png_iter.set(d_x, d_y);
        return;
    }

    int found = 0;

    for(itpub_cap_ImgUtil::rgb_list_iter it = list.begin(); it!=list.end(); it++)
    {
        rgb_t* t_rgb = &(*it);
        if(t_rgb->r == d_left.r && t_rgb->g ==d_left.g && t_rgb->b == d_left.b)
        {
            t_rgb->count++;
            found = 1; 
            break;
        }
    }

    if(found==0)
    {
        list.push_back(d_left);    
    }
    png_iter.set(d_x, d_y);
}

int itpub_cap_ImgUtil::top_colors(itpub_cap_PngPixelIter& png_iter, rgb_list& list)
{
    for(png_iter.reset(); png_iter.next(); ++png_iter)
    {
        rgb_t d_rgb;
        d_rgb.r = png_iter.r();
        d_rgb.g = png_iter.g();
        d_rgb.b = png_iter.b();

        if(is_black(d_rgb))
        {
            //The black color is the shadow. 
            //So, the black color should be at right or bottom
            check_left(png_iter, list);
            check_top(png_iter, list);
        }
    }
    std::sort(list.begin(), list.end(), rgb_compare);
    return 0;
}

int itpub_cap_ImgUtil::split_by_color(itpub_cap_PngPixelIter& png_iter, const rgb_t& rgb, captcha_word& word)
{
    int min_x;
    int min_y;
    int max_x;
    int max_y;

    min_x=min_y=max_x=max_y=0;

    for(png_iter.reset(); png_iter.next(); ++png_iter)
    {
        if(is_interfere(png_iter, png_iter.x(), png_iter.y())) continue;
        if(png_iter.r()==rgb.r && png_iter.g()==rgb.g && png_iter.b()==rgb.b)
        {
            if(min_x==0)
            {
                min_x = png_iter.x();
                min_y = png_iter.y();
            }else
            {
                min_x = std::min(png_iter.x(), min_x);
                min_y = std::min(png_iter.y(), min_y);    
            }
            max_x = std::max(png_iter.x(), max_x);
            max_y = std::max(png_iter.y(), max_y);
        }   
    }

    //printf("min_x=%d, min_y=%d, max_x=%d, max_y=%d\n", min_x, min_y, max_x, max_y);

    word.width=max_x-min_x+1;
    word.height=max_y-min_y+1;
    word.offset=min_x;

    for(png_iter.set(min_x, min_y); png_iter.y()<=max_y; ++png_iter)
    {
        captcha_word::row_t row;
        for(;png_iter.x()<max_x+1;++png_iter){
            if(png_iter.r()==rgb.r && png_iter.g()==rgb.g && png_iter.b()==rgb.b)
            {
                row.push_back(1); 
            }else
            {
                row.push_back(0);
            }
        }
        png_iter.set(min_x-1, png_iter.y()+1);
        word.matrix.push_back(row);
    }
    return min_x;
}
