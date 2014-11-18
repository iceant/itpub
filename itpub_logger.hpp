#ifndef ITPUB_LOGGER_H
#define ITPUB_LOGGER_H
#include<string>
#include<iostream>
#include<ctime>
#include<fstream>
#include<stdarg.h>
#include<cstdlib>

namespace PointCX{

    using namespace std;

    class Channel
    {
        friend class Trace;
        ostream* d_trace;

        public:
        Channel(ostream* o=&cout):d_trace(o){}
        void reset(ostream* o){d_trace = o;}

    };

    class Trace
    {
        private:
            std::string* d_name;
            Channel* d_channel;
            ofstream ofs;
            Channel DEFAULT_FILE_CHANNEL;

        public:
            static bool isTraceActive;

            Trace(const char* name, Channel* c=NULL)
            {
                if(isTraceActive)
                {
                    d_name = new std::string(name);
                    if(c==NULL)
                    {
                        ofs.open("itpub.log", ofstream::out|ofstream::app);
                        DEFAULT_FILE_CHANNEL.reset(&ofs);
                        d_channel = &DEFAULT_FILE_CHANNEL;
                    }
                    else
                        d_channel = c;
                    if(d_channel->d_trace)
                    {
                        *d_channel->d_trace << "[BEGIN] " << *d_name << endl; 
                    }
                }
            }

            ~Trace()
            {
                if(isTraceActive)
                {
                    if(d_channel->d_trace){
                        *d_channel->d_trace << "[END] " << *d_name << std::endl;
                    }
                    delete d_name;
                }
            }

            void append_time()
            {
                if(isTraceActive)
                {
                    if(d_channel->d_trace)
                    {
                        const time_t t = time(NULL);
                        char str[19];
                        struct tm *ptr;
                        ptr = localtime(&t);
                        strftime(str, 20, "%F %T", ptr);
                        *d_channel->d_trace << "\t[" << str << "] " << *d_name << "=> " ;
                    }
                }
            }

            void debug(const char* fmt, ...)
            {
                if(isTraceActive)
                {
                    if(d_channel->d_trace)
                    {
                        append_time();
                        va_list ap;
                        int size = 1024;
                        char *buffer=NULL;
                        if( (buffer = (char*)malloc(size))==NULL)
                        {
                            return;
                        }
                        bool isMemOk=true;
                        while(true)
                        {
                            va_start(ap, fmt);
                            int nchars = vsnprintf(buffer, size, fmt, ap);
                            va_end(ap);
                            if(nchars>-1 && nchars<size)
                            {
                                isMemOk = true;
                                break;
                            }
                            //---------------------
                            //buffer is not enough
                            size = size*2;
                            buffer = (char*)realloc(buffer, size);
                            if(buffer==NULL)
                            {
                                isMemOk=false;
                                break;
                            }
                        }
                        if(isMemOk)
                        {
                            *d_channel->d_trace << buffer;        
                        }
                        else{
                            *d_channel->d_trace << "[FAILED] Allocate " << size << " memory" << std::endl;
                        }
                        if(buffer!=NULL)
                        {
                            free(buffer);
                            buffer=NULL;
                        }
                    }
                }
            }
    };

}//END namespace PointCX
#endif //ITPUB_LOGGER_H
