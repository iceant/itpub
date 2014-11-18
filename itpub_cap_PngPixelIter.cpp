#include "itpub_cap_PngPixelIter.h"
#include <png.h>
#include <cstring>
#include <stdexcept>

// ===========================================================================
// itpub_cap_PngPixelIter_i
// ===========================================================================

class itpub_cap_PngPixelIter_i
{
public:
    FILE *fp;
    png_bytep *row_pointers;
    png_structp png_ptr;
    png_infop info_ptr;
    int width;
    int height;
    int color_type;
    int x;
    int y;

    const int PNG_BYTES_TO_CHECK = 4;

    itpub_cap_PngPixelIter_i(const char* pngFile)
        :png_ptr(png_create_read_struct(PNG_LIBPNG_VER_STRING, 0, 0, 0)),
        width(0),
        height(0),
        color_type(0),
        x(0),
        y(0)
    {
        fp = fopen(pngFile, "rb");
        if(fp==NULL)
        {
            char buf[100]={0};
            sprintf(buf, "Can not open %s\n\r", pngFile);
            throw std::runtime_error(buf);
        }
        info_ptr = png_create_info_struct(png_ptr);
        setjmp(png_jmpbuf(png_ptr));
        char buf[PNG_BYTES_TO_CHECK];
        int tmp = fread(buf, 1, PNG_BYTES_TO_CHECK, fp);
        if(tmp<PNG_BYTES_TO_CHECK)
        {
            fclose(fp);
            png_destroy_read_struct(&png_ptr, &info_ptr,0);
            throw std::runtime_error("Error PNG format!");
        }
        tmp = png_sig_cmp((png_bytep)buf, (png_size_t)0, PNG_BYTES_TO_CHECK);
        if(tmp!=0)
        {
            fclose(fp);
            png_destroy_read_struct(&png_ptr, &info_ptr,0);
            throw std::runtime_error("Error PNG SIG!");
        }
        rewind(fp);
        png_init_io(png_ptr, fp);
        png_read_png(png_ptr, info_ptr, PNG_TRANSFORM_EXPAND, 0);
        color_type = png_get_color_type(png_ptr, info_ptr);
        width = png_get_image_width(png_ptr, info_ptr);
        height = png_get_image_height(png_ptr, info_ptr);
        row_pointers = png_get_rows(png_ptr, info_ptr);
    }

    ~itpub_cap_PngPixelIter_i()
    {
        png_destroy_read_struct(&png_ptr, &info_ptr, 0);

        if(fp!=NULL)
        {
            fclose(fp);
            fp = NULL;
        }
    }
};


// ===========================================================================
// itpub_cap_PngPixelIter
// ===========================================================================
    itpub_cap_PngPixelIter::itpub_cap_PngPixelIter(const char* pngFile)
:d_this(new itpub_cap_PngPixelIter_i(pngFile))
{}

itpub_cap_PngPixelIter::~itpub_cap_PngPixelIter()
{
    if(d_this!=NULL)
    {
        delete d_this;
        d_this = NULL;
    }
}

int itpub_cap_PngPixelIter::isValidFile() const
{
    return d_this->fp!=NULL?1:0;
}


int itpub_cap_PngPixelIter::width() const
{
    return d_this->width;
}

int itpub_cap_PngPixelIter::height() const
{
    return d_this->height;
}

void itpub_cap_PngPixelIter::reset()
{
    //rewind(d_this->fp);
    d_this->x=0;
    d_this->y=0;
}

itpub_cap_PngPixelIter& itpub_cap_PngPixelIter::set(const int x, const int y)
{
    d_this->x = x;
    d_this->y = y;
    return *this;
}

itpub_cap_PngPixelIter& itpub_cap_PngPixelIter::operator++()
{
    if(d_this->x < d_this->width-1)
    {
        d_this->x++;
    }else
    {
        d_this->x = 0;
        if(d_this->y < d_this->height)
        {
            d_this->y++;
        }
    }
    return *this;
}

bool itpub_cap_PngPixelIter::next() const
{
    return d_this->y < d_this->height;
}

int itpub_cap_PngPixelIter::x() const
{
    return d_this->x;
}

int itpub_cap_PngPixelIter::y() const
{
    return d_this->y;
}

unsigned itpub_cap_PngPixelIter::r() const
{
    switch(d_this->color_type)
    { 
        case PNG_COLOR_TYPE_RGB_ALPHA:
            return d_this->row_pointers[d_this->y][d_this->x*4];
            break;
        case PNG_COLOR_TYPE_RGB:
            return d_this->row_pointers[d_this->y][d_this->x*3];
            break;
        default:
            return -1;
    }
}


unsigned itpub_cap_PngPixelIter::g() const
{
    switch(d_this->color_type)
    { 
        case PNG_COLOR_TYPE_RGB_ALPHA:
            return d_this->row_pointers[d_this->y][d_this->x*4+1];
            break;
        case PNG_COLOR_TYPE_RGB:
            return d_this->row_pointers[d_this->y][d_this->x*3+1];
            break;
        default:
            return -1;
    }
}

unsigned itpub_cap_PngPixelIter::b() const
{
    switch(d_this->color_type)
    { 
        case PNG_COLOR_TYPE_RGB_ALPHA:
            return d_this->row_pointers[d_this->y][d_this->x*4+2];
            break;
        case PNG_COLOR_TYPE_RGB:
            return d_this->row_pointers[d_this->y][d_this->x*3+2];
            break;
        default:
            return -1;
    }
}
