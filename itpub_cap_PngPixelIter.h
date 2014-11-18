#ifndef ITPUB_CAP_PNGPIXELITER_HEAD
#define ITPUB_CAP_PNGPIXELITER_HEAD

class itpub_cap_PngPixelIter_i;
class itpub_cap_PngPixelIter
{
    itpub_cap_PngPixelIter_i *d_this;
private:
    itpub_cap_PngPixelIter(const itpub_cap_PngPixelIter&);
    itpub_cap_PngPixelIter& operator=(const itpub_cap_PngPixelIter&);

public:
    itpub_cap_PngPixelIter(const char* pngFile);
    ~itpub_cap_PngPixelIter();

    void reset();
        // Return to the pixel(0,0)

    int isValidFile() const;
        // return non-zero when the file exist and readable. else 0

    int width() const;
        // PNG file size

    int height() const;
        // PNG file height

    bool next()const;

    itpub_cap_PngPixelIter& operator++();

    itpub_cap_PngPixelIter& set(const int x, const int y);

    int x() const;
        // Get current pixel position in x

    int y() const;
        // Get current pixel position in y

    unsigned r() const ;
    unsigned g() const ;
    unsigned b() const ;

};
#endif // ITPUB_CAP_PNGPIXELITER_HEAD
