#ifndef ITPUB_HTTP_HEADER
#define ITPUB_HTTP_HEADER

#ifndef ITPUB_CURL_HEADER
#define ITPUB_CURL_HEADER
#include <curl/curl.h>
#endif //ITPUB_CURL_HEADER

#ifndef ITPUB_STD_CSTDIO_H
#define ITPUB_STD_CSTDIO_H
#include <cstdio>
#endif //ITPUB_STD_CSTDIO_H


#ifndef ITPUB_STD_STRING_H
#define ITPUB_STD_STRING_H
#include <string>
#endif //ITPUB_STD_STRING_H

#ifndef ITPUB_STD_VECTOR_H
#define ITPUB_STD_VECTOR_H
#include <vector>
#endif //ITPUB_STD_VECTOR_H
#include <iostream>
#include <stdexcept>
#include <boost/thread.hpp>

namespace PointCX
{
    //=========================================================================
    // Request Policy
    //=========================================================================
    class itpub_http_PostPolicy
    {
        struct curl_httppost* formpost = NULL;
        struct curl_httppost* lastptr  = NULL;

        public:
        typedef std::pair<std::string, std::string> item_t;
        typedef std::vector<item_t> ValueType;
        typedef ValueType & ReferenceType;
        typedef ValueType const & ConstReferenceType;
        typedef ValueType * PointerType;

        void form_add(ConstReferenceType name_value_list)
        {
            for(ValueType::const_iterator it=name_value_list.begin();
                    it!=name_value_list.end();
                    ++it){
                curl_formadd(&formpost, &lastptr, 
                        CURLFORM_COPYNAME, it->first.c_str(),
                        CURLFORM_COPYCONTENTS, it->second.c_str(),
                        CURLFORM_END);
            }
        }

        void form_add(std::string& key, std::string& value)
        {
            curl_formadd(&formpost, &lastptr, 
                    CURLFORM_COPYNAME, key.c_str(),
                    CURLFORM_COPYCONTENTS, value.c_str(),
                    CURLFORM_END);
        }

        void form_add(const char* key, const char* value)
        {
            curl_formadd(&formpost, &lastptr, 
                    CURLFORM_COPYNAME, key,
                    CURLFORM_COPYCONTENTS, value,
                    CURLFORM_END);
        }
        void setopt(CURL* curl){
            curl_easy_setopt(curl, CURLOPT_HTTPPOST, formpost);
        }

        protected:
        //Only the extended class can delete this object
        ~itpub_http_PostPolicy(){
            //std::cout << "~itpub_http_PostPolicy" << std::endl;
            if(formpost!=NULL){
                curl_formfree(formpost);
                formpost = NULL;
            }
        }
    }; 

    class itpub_http_GetPolicy
    {
        public:
            inline void setopt(CURL* curl)
            {
                curl_easy_setopt(curl, CURLOPT_POST, false);
            }
    };


    //=========================================================================
    // Cookie Policy
    //=========================================================================
    class itpub_http_CookiePolicy
    {
        private:
            std::string d_cookie;
        public:
            void setCookie(const char* cookie)
            {
                d_cookie=cookie;
            }
            inline void setopt(CURL *curl)
            {
                curl_easy_setopt(curl, CURLOPT_COOKIEFILE, d_cookie.c_str());
                curl_easy_setopt(curl, CURLOPT_COOKIEJAR, d_cookie.c_str());
            }

        protected:
            ~itpub_http_CookiePolicy()
            {
                //std::cout << "~itpub_http_CookiePolicy" << std::endl;
            }
    };

    //=========================================================================
    // Output Policy
    //=========================================================================
    class itpub_http_EmptyOutputPolicy
    {
        public:
            void setopt(CURL *curl)
            {
                curl_easy_setopt(curl, CURLOPT_NOBODY, 1L);
                curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, NULL);
                curl_easy_setopt(curl, CURLOPT_WRITEDATA, NULL);
            }

            void reset(){
                return;
            }
    };

    class itpub_http_OutputStrPolicy
    {
        protected:
            inline static size_t writeStrFunc(void* ptr, size_t size, size_t nmemb, void* stream)
            {
                std::string *str = dynamic_cast<std::string*>((std::string *)stream);
                if(NULL==str || NULL ==ptr)
                {
                    return -1;
                }
                char *pData = (char*) ptr;
                str->append(pData, size * nmemb);
                return nmemb;
            }

            std::string* d_content;

        public:
            itpub_http_OutputStrPolicy():d_content(NULL){}

            void setOutput(std::string& output)
            {
                d_content = &output;
            }

            void setOutput(std::string* content)
            {
                d_content = content;
            }

            std::string* getOutput(){
                return d_content;
            }

            const std::string* getOutput() const
            {
                return d_content;
            }

            void reset()
            {
                d_content->clear();
            }

            void setopt(CURL *curl)
            {
                curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeStrFunc);
                curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void*)d_content);
            }

        protected:
            ~itpub_http_OutputStrPolicy()
            {
                d_content =NULL;
            }
    };


    class itpub_http_OutputFilePolicy
    {
        public:
            static size_t writeFunc(void* ptr, size_t size, size_t nmemb, void* stream)
            {
                size_t written;
                written = fwrite(ptr, size, nmemb, (FILE*)stream);
                return written;
            }
        private:
            std::string fileName;
            FILE* fp;
        public:

            itpub_http_OutputFilePolicy():fp(NULL){}

            int setOutput(const char* file)
            {
                fileName = file;
                fp = fopen(file, "wb");
                if(fp==NULL){
                    return -1;
                }
                return 0;
            }

            int setOutput(const std::string& file)
            {
                fileName = file;
                fp = fopen(file.c_str(), "wb");
                if(fp==NULL){
                    return -1;
                }
                return 0;
            }

            void reset()
            {
                if(fp!=NULL)
                    rewind(fp);
            }

            FILE* getOutput() const
            {
                return fp;
            }

            inline void setopt(CURL *curl)
            {
                curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1);
                curl_easy_setopt(curl, CURLOPT_READFUNCTION, NULL);
                curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeFunc);
                curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
            }

            inline void release(){
                if(fp)
                {
                    fclose(fp);
                    fp = NULL;
                }
            }

        protected:
            ~itpub_http_OutputFilePolicy()
            {
                release();
            }
    };

    //=========================================================================
    // itpub_http Header Policy
    //=========================================================================
    class itpub_http_HeaderPolicy{
        private:
            struct curl_slist *header;
            std::string header_content;
            bool d_isGetInHeader;
        public:
            inline static size_t writeStrFunc(void* ptr, size_t size, size_t nmemb, void* stream)
            {
                std::string *str = dynamic_cast<std::string*>((std::string *)stream);
                if(NULL==str || NULL ==ptr)
                {
                    return -1;
                }
                char *pData = (char*) ptr;
                str->append(pData, size * nmemb);
                return nmemb;
            }


            itpub_http_HeaderPolicy():header(NULL), header_content(""), d_isGetInHeader(false){}
            ~itpub_http_HeaderPolicy()
            {
                curl_slist_free_all(header);
            }

            void setGetInHeaders(bool isGet){
                d_isGetInHeader = isGet; 
            }

            int add_header(const char* value){
                if(!value) return -1;
                header = curl_slist_append(header, value);
                return 0;
            }

            std::string& getInHeaderContent(){
                return header_content;
            }

            inline void setopt(CURL* curl)
            {
                curl_easy_setopt(curl, CURLOPT_HTTPHEADER, header);
                if(d_isGetInHeader){
                    curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, writeStrFunc);
                    curl_easy_setopt(curl, CURLOPT_HEADERDATA, &header_content);
                }
            }
    };

    //=========================================================================
    // itpub_http Timout Policy
    //=========================================================================
    class itpub_http_TimeOutPolicy
    {
        private:
            long d_opt_timeout;
            long d_con_timeout;

        public:
            itpub_http_TimeOutPolicy():d_opt_timeout(60), d_con_timeout(30){}
            ~itpub_http_TimeOutPolicy()
            {
                //std::cout << "~itpub_http_TimeOutPolicy" << std::endl;
            }

            void setTimeout(long t)
            {
                d_opt_timeout = t;
            }

            void setConnectTimeout(long t)
            {
                d_con_timeout = t;
            }

        protected:
            inline void setopt(CURL* curl){
                curl_easy_setopt(curl, CURLOPT_TIMEOUT, d_opt_timeout);
                curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, d_con_timeout);
            }
    };


    //=========================================================================
    // itpub_http Follow Location Policy
    //=========================================================================
    class itpub_http_FollowLocationPolicy
    {
        private:
            int d_maxRedirs;
            bool d_isFollow;
        public:
            itpub_http_FollowLocationPolicy():d_maxRedirs(5), d_isFollow(true){}

            void setMaxRedirs(int max)
            {
                d_maxRedirs = max;
            }

            void setFollowLocation(bool isfollow)
            {
                d_isFollow = isfollow;
            }

            inline void setopt(CURL* curl)
            {
                curl_easy_setopt(curl, CURLOPT_MAXREDIRS, d_maxRedirs);
                curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, d_isFollow?1:0);
            }
    };


    class itpub_http_ProgressPolicy
    {
        private:
            bool d_isShowProgress;
        public:
            itpub_http_ProgressPolicy():d_isShowProgress(false){}

            void setShowProgress(bool show){
                d_isShowProgress = show;
            }

            inline void setopt(CURL* curl)
            {
                curl_easy_setopt(curl, CURLOPT_NOPROGRESS, d_isShowProgress?0L:1L);
            }
    };


    class itpub_http_DebugPolicy
    {
        private:
            bool d_isDebug;
            static int debugFunc(CURL *, curl_infotype itype, char * pData, size_t size, void *)
            {
                if(itype == CURLINFO_TEXT)
                {
                    printf("[TEXT]%s\n", pData);
                }
                else if(itype == CURLINFO_HEADER_IN)
                {
                    printf("[HEADER_IN]%s\n", pData);

                }
                else if(itype == CURLINFO_HEADER_OUT)
                {
                    printf("[HEADER_OUT]%s\n", pData);

                }
                else if(itype == CURLINFO_DATA_IN)
                {
                    printf("[DATA_IN]%s\n", pData);

                }
                else if(itype == CURLINFO_DATA_OUT)
                {
                    printf("[DATA_OUT]%s\n", pData);

                }
                return 0;
            }

        public:
            void setDebug(bool isDebug)
            {
                d_isDebug=isDebug;
            }

            itpub_http_DebugPolicy():d_isDebug(false){}

            inline void setopt(CURL* curl)
            {
                if(d_isDebug)
                {
                    curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
                    curl_easy_setopt(curl, CURLOPT_DEBUGFUNCTION, debugFunc);
                }
            }
    };

    //=========================================================================
    // Perform Policy
    template< class OutputPolicy >
    class itpub_http_KeepTryOnFailPerformPolicy
    {
        private:
            int d_sleep_second;
        public:
            itpub_http_KeepTryOnFailPerformPolicy():d_sleep_second(3){}
            void setSleepWhenPerformFailure(int second)
            {
                d_sleep_second = second;
            }
        protected:
            inline CURLcode perform(OutputPolicy& op, CURL* curl)
            {
                CURLcode ret = curl_easy_perform(curl);
                int timeout = 60;
                int ntry=1;
                while(ret!=CURLE_OK)
                {
                    if(ret==CURLE_OPERATION_TIMEDOUT){
                        ntry = ntry + 1;
                        std::cout << "KeepTryOnFailPerformPolicy setTimeout:" << timeout * ntry << std::endl;
                        curl_easy_setopt(curl, CURLOPT_TIMEOUT, timeout*ntry);
                    }
                    op.reset();
                    boost::this_thread::sleep(boost::posix_time::seconds(d_sleep_second));
                    ret = curl_easy_perform(curl);
                }
                return ret;
            }
    };

    template<typename OutputPolicy >
    class itpub_http_NormalPerformPolicy
    {
        protected:
            inline CURLcode perform(OutputPolicy& op, CURL* curl)
            {
                return curl_easy_perform(curl);
            }
    };

    //=========================================================================
    // itpub_http Host 
    //=========================================================================
    template
        < 
        class OutputPolicy, 
              class RequestPolicy = itpub_http_PostPolicy, 
              template <class> class PerformPolicy = itpub_http_NormalPerformPolicy,
              class HeaderPolicy = itpub_http_HeaderPolicy,
              class CookiePolicy = itpub_http_CookiePolicy,
              class TimeOutPolicy = itpub_http_TimeOutPolicy,
              class FollowLocationPolicy = itpub_http_FollowLocationPolicy,
              class ProgressPolicy=itpub_http_ProgressPolicy,
              class DebugPolicy=itpub_http_DebugPolicy
                  >
                  class itpub_http_Client
                  :
                      public OutputPolicy
                      , public RequestPolicy
                      , public HeaderPolicy
                      , public TimeOutPolicy
                      , public CookiePolicy
                      , public FollowLocationPolicy
                      , public ProgressPolicy
                      , public DebugPolicy
                      , public PerformPolicy<OutputPolicy>
    {
        private:
            itpub_http_Client(const itpub_http_Client&);
            itpub_http_Client& operator=(const itpub_http_Client&);
        public:
            itpub_http_Client(){
            }

            int request(const char* url)
            {
                CURL* curl;
                curl = curl_easy_init();
                if(curl==NULL){
                    return CURLE_FAILED_INIT;
                }
                CURLcode res;
                curl_easy_setopt(curl, CURLOPT_URL, url);
                HeaderPolicy::setopt(curl);
                RequestPolicy::setopt(curl);
                CookiePolicy::setopt(curl);
                OutputPolicy::setopt(curl);
                TimeOutPolicy::setopt(curl);
                FollowLocationPolicy::setopt(curl);
                ProgressPolicy::setopt(curl);
                DebugPolicy::setopt(curl);
                res = PerformPolicy<OutputPolicy>::perform(*this, curl);
                // res = curl_easy_perform(curl);
                //std::cout << "Code " << res << " " << url << std::endl;
                curl_easy_cleanup(curl);
                return res;
            }

            ~itpub_http_Client(){
            }
    };

}
#endif //ITPUB_HTTP_HEADER
