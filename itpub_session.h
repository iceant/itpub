#ifndef ITPUB_SESSION_HEADER
#define ITPUB_SESSION_HEADER


#ifndef ITPUB_STD_STRING_H
#define ITPUB_STD_STRING_H
#include <string>
#endif //ITPUB_STD_STRING_H


#ifndef ITPUB_STD_VECTOR_H
#define ITPUB_STD_VECTOR_H
#include <vector>
#endif //ITPUB_STD_VECTOR_H

namespace PointCX
{
    class itpub_Session_i;
    class itpub_Session
    {
        private:
            itpub_Session_i *d_this;
            itpub_Session(const itpub_Session&);
            itpub_Session& operator=(const itpub_Session&);

        public:
            typedef std::vector<std::string> url_list_t;

            itpub_Session();
            ~itpub_Session();

            // Methods
            int login(const char* username, const char* password);
            int login(const std::string& username, const std::string& password);

            int captcha(const char* url, 
                    const char* imgDir, 
                    const char* captcha_lib_path, 
                    std::string& code_ret);

            int download(const char* url, const char* dir);

            int scanThread(const char* thread_url, url_list_t& attachments);

            int scanForum(const char* forum_url, url_list_t& thread_urls);

            int downloadThread(const char* thread_url, const char* dir);

            int getThreadId(const std::string& thread_url, std::string ret);

    };
}
#endif //ITPUB_SESSION_HEADER
