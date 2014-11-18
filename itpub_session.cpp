#include "itpub_session.h"
#include "itpub_config.h"
#include "itpub_http.h"
#include "itpub_captcha.h"
#include "itpub_dir.hpp"
#include <sstream>
//#include <iconv.h>
#include <cstring>
#include <cstdlib>
#include "itpub_logger.hpp"
#include <boost/filesystem.hpp>
#include "itpub_utf8.h"

namespace PointCX
{
    namespace bf=boost::filesystem;

    class itpub_Session_i
    {
        public:
            typedef itpub_http_Client<itpub_http_OutputStrPolicy> http_str_post_client_t;
            typedef itpub_http_Client<itpub_http_OutputStrPolicy, itpub_http_GetPolicy, itpub_http_KeepTryOnFailPerformPolicy> http_str_get_client_t;
            typedef itpub_http_Client<itpub_http_OutputFilePolicy> http_file_post_client_t;
            typedef itpub_http_Client<itpub_http_OutputFilePolicy, itpub_http_GetPolicy> http_file_get_client_t;
            typedef itpub_http_Client<itpub_http_EmptyOutputPolicy, itpub_http_GetPolicy, itpub_http_KeepTryOnFailPerformPolicy> http_empty_get_client_t;

            http_str_post_client_t login_client;
            http_str_get_client_t  http_get;

            static const char* COOKIE;

            int saveSecImg(const char* imgUrl, const char* localFile, const char* referer)
            {
                http_file_get_client_t http_file;
                std::stringstream ss;
                ss << "Referer: " << referer;
                http_file.add_header(ss.str().c_str());
                http_file.add_header("User-Agent: Mozilla/5.0 (Windows NT 6.3; WOW64; Trident/7.0; rv:11.0) like Gecko");
                http_file.add_header("Accept: image/png, image/svg+xml, image/*;q=0.8, */*;q=0.5");
                http_file.add_header("Connection: Keep-Alive");
                http_file.add_header("Host: www.itpub.net");
                http_file.setDebug(false);
                http_file.setCookie(COOKIE);
                http_file.setOutput(localFile);
                http_file.setShowProgress(false);
                return http_file.request(imgUrl);
            }

            int get_aid(const char* url, std::string& ret)
            {
                std::string d_url = url;
                size_t start = d_url.find("aid=");
                if(start!=std::string::npos){
                    size_t end = d_url.find("&", start+4);
                    ret = d_url.substr(start+4, end-start-4);
                    return 0;
                }
                return -1;
            }

            int get_real_dl_url(const char* url, std::string& ret){
                std::string aid;
                int r = get_aid(url, aid);
                if(r==-1)
                {
                    ret = "";
                    return -1;
                }
                std::stringstream ss;
                ss<<"http://www.itpub.net/forum.php?mod=attachment&aid="<< aid <<"&fid=61"; 
                ret = ss.str();
                return 0;
            }

            // inline std::string UTF8ToGBK(char* src)
            // {
                // std::string ans;
                // int len = strlen(src)*2+1;
                // char *dst = (char *)malloc(len);
                // if(dst == NULL)
                // {
                    // return ans;
                // }
                // memset(dst, 0, len);
                // char *in = src;
                // char *out = dst;
                // size_t len_in = strlen(src);
                // size_t len_out = len;

                // iconv_t cd = iconv_open("ISO-8859-1", "UTF8");
                // if ((iconv_t)-1 == cd)
                // {
                    // printf("init iconv_t failed\n");
                    // free(dst);
                    // return ans;
                // }
                // int n = iconv(cd, &in, &len_in, &out, &len_out);
                // if(n<0)
                // {
                    // printf("iconv failed\n");
                // }
                // else
                // {
                    // ans = dst;
                // }
                // free(dst);
                // iconv_close(cd);
                // return ans;
            // }

            inline void replace_str(const std::string& str
                    , const std::string& src
                    , const std::string& dest
                    , std::string& ret)
            {
                std::string result=str;
                while(true)
                {
                    std::string::size_type pos = result.find(src);
                    if(pos!=std::string::npos)
                    {
                        result.replace(pos, src.size(), dest);
                    }
                    else
                        break;
                }
                ret = result;
            }

            inline int u_fname(const std::string& fullFilePath, std::string& ret)
            {
                Trace log("itpub_session.u_fname");
                log.debug("Generate unique filename for [%s]\n", fullFilePath.c_str());
                enum{OK=0, ERR_NO_PATH=-1};
                bf::path a_path(fullFilePath);
                if(!bf::exists(a_path)){
                    ret = a_path.c_str();
                    return OK;
                }
                bf::path parent_dir = a_path.parent_path();
                std::string ext = a_path.extension().c_str();
                std::string fileName = a_path.filename().c_str();
                std::string::size_type pos = fileName.rfind(".");
                fileName = fileName.substr(0, pos);
                int i=0;
                do{
                    std::stringstream ss;
                    ss << fileName << "("<< (++i) << ")" << ext;
                    a_path = parent_dir/ss.str();
                    log.debug("Path:%s\n", a_path.c_str());
                }while(bf::exists(a_path)); 
                ret = a_path.c_str();
                return OK;
            }

            inline void formatFileName(std::string& fileName){
                Trace log("itpub_session.formatFileName");
                log.debug("Formating filename: %s\n", fileName.c_str());
                char tmp_fileName[255];
                gbk_to_utf8(fileName.c_str(), fileName.size(), tmp_fileName, 255);
                log.debug("After to GBK:%s\n", tmp_fileName);
                std::string tmp_fileName_str =  tmp_fileName;
                replace_str(tmp_fileName_str, "&amp;", "&", tmp_fileName_str);
                replace_str(tmp_fileName_str, "&#039;", "—", tmp_fileName_str);
                replace_str(tmp_fileName_str, "&lt;", "<", tmp_fileName_str);
                replace_str(tmp_fileName_str, "&gt;", ">", tmp_fileName_str);
                replace_str(tmp_fileName_str, "\\", "_", tmp_fileName_str);
                replace_str(tmp_fileName_str, "/", "_", tmp_fileName_str);
                replace_str(tmp_fileName_str, ":", "_", tmp_fileName_str);
                replace_str(tmp_fileName_str, "?", "_", tmp_fileName_str);
                replace_str(tmp_fileName_str, "*", "_", tmp_fileName_str);
                replace_str(tmp_fileName_str, "<", "_", tmp_fileName_str);
                replace_str(tmp_fileName_str, ">", "_", tmp_fileName_str);
                replace_str(tmp_fileName_str, "|", "_", tmp_fileName_str);
                replace_str(tmp_fileName_str, ";", "_", tmp_fileName_str);
                replace_str(tmp_fileName_str, " ", "_", tmp_fileName_str);
                fileName = tmp_fileName_str;
                log.debug("Return formated filename: %s\n", fileName.c_str());
            }

            int getFileName(const char* url, std::string& ret)
            {
                http_empty_get_client_t http;
                std::stringstream ss;
                ss << "Referer: " << url;
                http.add_header(ss.str().c_str());
                http.setGetInHeaders(true);
                http.add_header("User-Agent: Mozilla/5.0 (Windows NT 6.3; WOW64; Trident/7.0; rv:11.0) like Gecko");
                http.add_header("Accept: text/html, application/xhtml+xml, */*");
                http.add_header("Connection: Keep-Alive");
                http.add_header("Cache-Control: no-cache");
                http.add_header("Host: www.itpub.net");
                http.add_header("Content-Type: application/x-www-form-urlencoded");
                http.setDebug(false);
                http.setCookie(COOKIE);
                http.setFollowLocation(true);
                std::string header;
                std::string real_url;
                get_real_dl_url(url, real_url);
                int r = http.request(real_url.c_str());
                if(r==0){
                    header = http.getInHeaderContent();
                    size_t start = header.find("filename=\"");
                    if(start!=std::string::npos){
                        size_t end = header.find("\"", start + 10);
                        ret = header.substr(start+10, end - start-10);
                        formatFileName(ret);
                        return 0; 
                    }
                }
                return -1;
            }

            inline int download_exec(const char* url, 
                    const char* localFile, const int timeout)
            {
                http_file_get_client_t http_file;
                std::stringstream ss;
                ss << "Referer: " << url;
                http_file.add_header(ss.str().c_str());
                http_file.add_header("User-Agent: Mozilla/5.0 (Windows NT 6.3; WOW64; Trident/7.0; rv:11.0) like Gecko");
                //http_file.add_header("Accept: text/html, application/xhtml+xml, */*");
                //http_file.add_header("Accept-Language: en-US;q=0.5");
                //http_file.add_header("Connection: Keep-Alive");
                http_file.add_header("Cache-Control: no-cache");
                http_file.add_header("Host: www.itpub.net");
                //http_file.add_header("Content-Type: application/x-www-form-urlencoded");
                //http_file.setDebug(true);
                http_file.setCookie(COOKIE);
                http_file.setOutput(localFile);
                http_file.setShowProgress(true);
                http_file.setFollowLocation(true);
                std::string real_url;
                get_real_dl_url(url, real_url);
                return http_file.request(real_url.c_str());
            }

            inline int download(const char* url, 
                    const char* localFile)
            {
                Trace log("itpub_session.download(url, file)");
                log.debug("Download %s\n", localFile);
                int ntry = 1;
                int timeout = 60;
                int ret = -99; 
                while(ret!=CURLE_OK)
                {
                    bf::path file = localFile;
                    if(bf::exists(file) && bf::remove_all(file)==0)
                    {
                        log.debug("Delete %s failed!\n", file.c_str());
                    }
                    if(ret==CURLE_PARTIAL_FILE){
                        log.debug("Can not download the file:%s! The firl in url:%s is not send complete!\n", localFile, url);
                        break;
                    }
                    // if(ret!=-99)
                        // boost::this_thread::sleep(boost::posix_time::seconds(5));
                    if(ret==CURLE_OPERATION_TIMEDOUT){
                        ntry = ntry +1;
                        log.debug("Timeout with %d, retry %d with %d\n", timeout, ntry, ntry*timeout);
                        timeout = ntry*timeout;
                        //ret = download_exec(url, localFile, ntry*timeout); 
                    }
                    //std::cout << "download.return_code:" << ret << std::endl;
                    ret = download_exec(url, localFile, timeout);
                    log.debug("Return Code:%d\n", ret);
                }
                return ret;
            }
    };

    const char* itpub_Session_i::COOKIE = "itpub_cookie";


    //========================================================================
    //  itpub_Session
    //========================================================================
    itpub_Session::itpub_Session():d_this(new itpub_Session_i){}

    itpub_Session::~itpub_Session()
    {
        delete d_this;
    }

    int itpub_Session::login(const char* username, const char* password)
    {
        enum{OK=1, FAILED=-1};
        std::string res;
        d_this->login_client.setCookie(itpub_Session_i::COOKIE);
        d_this->login_client.setOutput(res);
        itpub_http_PostPolicy::ValueType name_value_list;
        name_value_list.push_back(std::make_pair("username", username));
        name_value_list.push_back(std::make_pair("password", password));
        d_this->login_client.form_add(name_value_list);
        int ret = d_this->login_client.request("http://www.itpub.net/member.php?mod=logging&action=login&loginsubmit=yes&infloat=yes&lssubmit=yes&inajax=1");
        if(res.find("succeedhandle")!=std::string::npos) return OK;
        if(res.find("errorhandle")!=std::string::npos) return FAILED;
        return ret;
    }

    int itpub_Session::login(const std::string& username, const std::string& password)
    {
        return login(username.c_str(), password.c_str());
    }

    inline int getFormhash(const std::string& content, std::string& ret)
    {
        std::string key = "formhash=";
        size_t start = content.find(key);
        if(start!=std::string::npos)
        {
            size_t end = content.find("\"", start+key.size());
            ret = content.substr(start + key.size(), end-start-key.size());
            return 0;
        }
        ret = "";
        return -1;
    }

    inline int getSecCode(const std::string& content, std::string& ret)
    {
        std::string key = "seccode('";
        size_t start = content.find(key);
        if(start!=std::string::npos)
        {
            size_t end = content.find("'", start + key.size()+1);
            start = start + key.size();
            ret = content.substr(start, end-start);
            return 0;
        }
        ret = "";
        return -1;
    }

    inline void getSecImgAddr(const std::string& seccode, std::string& ret)
    {
        std::stringstream ss;
        ss<< "http://www.itpub.net/misc.php?mod=seccode&action=update&idhash="<< seccode << "&inajax=1&ajaxtarget=seccode_"<< seccode;
        ret = ss.str(); 
    }

    inline int getSecUpdate(const std::string& content, std::string& ret)
    {
        std::string key = "&update=";
        size_t start = content.find(key);
        if(start!=std::string::npos)
        {
            size_t end = content.find("&", start + key.size()+1);
            ret = content.substr(start + key.size(), end-start-key.size());
            return 0;
        }
        ret = "";
        return -1;
    }

    inline int getSecImgUrl(const std::string& update, const std::string& seccode, std::string& ret)
    {
        std::stringstream ss;
        ss<< "http://www.itpub.net/misc.php?mod=seccode&update=" << update;
        ss<< "&idhash=" << seccode;
        ret = ss.str();
        return 0;
    }

    inline int getSecImgFile(const char* imgDir
            , const std::string& update
            , const std::string& seccode
            , std::string& ret)
    {
        std::stringstream ss;
        ss << imgDir << "/" << update << "_" << seccode << ".png";
        ret = ss.str();
        return 0;
    }

    int itpub_Session::captcha(const char* url, const char* dir, 
            const char* captcha_lib_path, 
            std::string& code_ret)
    {
        enum{CAPTCHA_NOT_EXIST=-1, CAPTCH_LIB_ERROR=-2, OK=0};
        std::string content;
        d_this->http_get.setOutput(content);
        d_this->http_get.setCookie(itpub_Session_i::COOKIE);
        int ret = d_this->http_get.request(url);
        if(ret!=0) return ret;
        std::string formhash;
        getFormhash(content, formhash);
        std::string seccode;
        getSecCode(content, seccode);
        std::string imgInfoUrl;
        getSecImgAddr(seccode, imgInfoUrl);
        content.clear();
        d_this->http_get.setOutput(content);
        ret = d_this->http_get.request(imgInfoUrl.c_str()); //res is the url now.
        std::string update;
        ret = getSecUpdate(content, update);
        std::string imgUrl;
        ret = getSecImgUrl(update, seccode, imgUrl);
        std::string imgFile;
        ret = getSecImgFile(dir, update, seccode, imgFile);

        ret = d_this->saveSecImg(imgUrl.c_str(), imgFile.c_str(), url);

        std::vector<record_t> records;
        if(!read_lib(captcha_lib_path, records))
            return CAPTCH_LIB_ERROR;
        std::vector<char> img_code;
        if(!read_img(imgFile, records, img_code))
        {
            return CAPTCHA_NOT_EXIST; 
        }
        for(std::vector<char>::iterator it=img_code.begin(); it!=img_code.end(); ++it)
        {
            code_ret+=*it;
        }

        return 0;
    }

    int itpub_Session::download(const char* url, const char* dir){
        enum{ERR_CANNOT_GET_ATTACHMENT=-90};
        Trace log("itpub_Session::download");
        bf::path path(dir);
        log.debug("Dir %s exist? %s\n", path.c_str(), bf::exists(path)?"yes":"no");
        if(!bf::exists(path))
        {
            bool b = bf::create_directory(path);
            if(!b)
                return -99;
        }
        std::string fileName;
        int r = d_this->getFileName(url, fileName);
        if(r==-1)
        {
            return ERR_CANNOT_GET_ATTACHMENT;
        }
        path = path/fileName;
        std::string path_str=path.c_str();
        d_this->u_fname(path_str, path_str);
        path = path_str;
        std::cout << std::endl << "Downloading... " << path << std::endl;
        log.debug("Downloading... %s\n", path.c_str());
        return d_this->download(url, path.c_str());
    }

    //-------------------------------------------------------------
    // Thead Functions
    //

    inline int getMaxPageNum(const std::string& content)
    {
        const std::string token = "class=\"nxt\"";
        size_t start = content.find(token);
        if(start!=std::string::npos)
        {
            size_t end;
            end = content.rfind("</a>", start);
            if(end!=std::string::npos){
                start = content.rfind(">", end);
                std::string num = content.substr(start+1, end-start-1);
                //... 15
                size_t ns = num.find("... ");
                if(ns!=std::string::npos)
                {
                    num = num.substr(4);
                }
                return atoi(num.c_str());
            }
        }
        return 1;
    }

    inline std::string getThreadIdInternal(const std::string& thread_url)
    {
        //std::cout << "Geting thread-id from " << thread_url << std::endl;
        //thread-xxxx-1-1.html
        size_t start = thread_url.find("thread-");
        if(start!=std::string::npos){
            size_t end = thread_url.find("-", start+7);
            std::string t = thread_url.substr(start+7, end-start-7);
            return t;
        }
        // http://www.itpub.net/forum.php?mod=viewthread&tid=1890524&extra=page%3D1%26filter%3Dauthor%26orderby%3Ddateline%26orderby%3Ddateline 
        start = thread_url.find("tid=");
        if(start!=std::string::npos){
            size_t end = thread_url.find("&", start+4);
            std::string t = thread_url.substr(start+4, end-start-4);
            return t;
        }
        return "";
    }

    inline int scanAttachments(const char* thread_url,
            itpub_Session::url_list_t& attachment_urls)
    {
        Trace log("itpub_session.scanAttachments");
        itpub_Session_i::http_str_get_client_t http;
        std::string content;
        http.setOutput(content);
        http.setCookie(itpub_Session_i::COOKIE);
        http.request(thread_url);

        std::string token = "\"attachment.php?aid=";
        size_t start = content.find(token);
        while(start!=std::string::npos){
            size_t end = content.find("\"", start+token.size());
            if(end!=std::string::npos)
            {
                std::string a_url = content.substr(start+1, end-start-1);
                if(a_url.find("fid")!=std::string::npos)
                {
                    attachment_urls.push_back(a_url);
                    log.debug("Find attachment:%s\n", a_url.c_str());
                }
            }
            start = content.find(token, end);
        }
        return 0;

    }

    int itpub_Session::scanThread(const char* thread_url, url_list_t& attachments)
    {
        Trace log("itpub_Session::scanThread");
        std::string thread_id = getThreadIdInternal(thread_url);
        std::stringstream ss1;
        ss1 << "http://www.itpub.net/thread-" << thread_id << "-1-1.html";
        itpub_Session_i::http_str_get_client_t http;
        std::string content;
        http.setOutput(content);
        http.setCookie(itpub_Session_i::COOKIE);
        http.request(ss1.str().c_str());

        int maxPageNum = getMaxPageNum(content);
        log.debug("maxPageNum=%d for Thread URL:%s\n", maxPageNum, ss1.str().c_str());
        for(int i=1; i<= maxPageNum; i++)
        {
            std::stringstream ss;
            ss << "http://www.itpub.net/thread-" << thread_id << "-" << i << "-1.html";
            std::cout << "Scanning attachments in thread " << ss.str() << std::endl;
            scanAttachments(ss.str().c_str(), attachments);
            log.debug("Scanned %d attachments in thread %s\n", attachments.size(), ss.str().c_str());
        }

        return 0;
    }

    inline int getForumId(const char* forum_url)
    {
        std::string url = forum_url;
        size_t start = url.find("fid=");
        if(start!=std::string::npos){
            size_t end = url.find("&", start+4);
            std::string n = url.substr(start+4, end-start-4);
            return atoi(n.c_str());
        }

        start = url.find("forum-");
        if(start!=std::string::npos)
        {
            size_t end = url.find("-", start+6);
            std::string n = url.substr(start+6, end-start-6);
            return atoi(n.c_str());
        }
        return -1;
    }

    inline void getForumUrl(const char* forum_url, const int page, std::string& ret){
        std::string url = forum_url;
        if(url.find("fid=")!=std::string::npos)
        {
            std::stringstream ss;
            int forum_id = getForumId(forum_url);
            ss<< "http://www.itpub.net/forum.php?mod=forumdisplay&fid="<< forum_id <<"&filter=author&orderby=dateline&page=" << page;
            ret = ss.str();
            return;
        } 
        else{
            std::stringstream ss;
            ss << "http://www.itpub.net/forum-" << getForumId(forum_url) << "-" <<page<<".html";
            ret = ss.str();
            return;
        }
    }

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

    inline bool isThreadInList(const std::string& thread_url, 
            const std::vector<std::string>& tids)
    {
        // Trace log("itpub_session.isThreadInList");
        std::string tid = getThreadIdInternal(thread_url);
        for(std::vector<std::string>::const_iterator cit = tids.begin();
                cit!=tids.end(); ++cit)
        {
            // log.debug("Compare %s VS %s\n", tid.c_str(), (*cit).c_str());
            if(tid.compare(*cit)==0)
            {
                return true;
            }
        }
        return false;
    }

    inline bool isSkipThread(const std::string& thread_url)
    {
        itpub_Config cfg;
        cfg.load("config.cfg");
        std::string skip_threads;
        cfg.get("threads.skip", skip_threads);
        std::vector<std::string> tids;
        split(skip_threads, ",", tids);
        std::vector<std::string> queued_tids;
        std::string queued_tids_str;
        cfg.get("threads.done", queued_tids_str);
        split(queued_tids_str, ",", queued_tids);
        if(isThreadInList(thread_url, tids) || isThreadInList(thread_url, queued_tids))
        {
            return true;
        }
        return false;
    }

    inline int scanThreadsInForumPage(std::string& forum_page_url, itpub_Session::url_list_t& thread_urls)
    {
        Trace log("itpub_session.scanThreadsInForumPage");
        log.debug("URL:%s\n", forum_page_url.c_str());
        itpub_Session_i::http_str_get_client_t http;
        std::string content;
        http.setOutput(content);
        http.setCookie(itpub_Session_i::COOKIE);
        http.setTimeout(30);
        http.request(forum_page_url.c_str());
        // std::string token = "class=\"xst\"";
        itpub_Config cfg;
        cfg.load("config.cfg");
        std::string skip_threads;
        cfg.get("threads.skip", skip_threads);
        std::vector<std::string> tids;
        split(skip_threads, ",", tids);
        std::vector<std::string> queued_tids;
        std::string queued_tids_str;
        cfg.get("threads.done", queued_tids_str);
        split(queued_tids_str,",", queued_tids);

        std::string token = "alt=\"attach";
        size_t start = content.find(token);
        while(start!=std::string::npos)
        {
            size_t url_start = content.rfind("href=\"", start);
            if(url_start!=std::string::npos){
                size_t url_end = content.find("\"", url_start+6);
                std::string thread_url = content.substr(url_start+6, url_end-url_start-6);
                //-----------------------------------
                // Skip 
                if(isThreadInList(thread_url, tids) || isThreadInList(thread_url, queued_tids))
                {
                    log.debug("Skipping thread %s\n", thread_url.c_str());
                }
                else
                {
                    log.debug("Find thread http://www.itpub.net/%s\n", thread_url.c_str());
                    thread_urls.push_back("http://www.itpub.net/"+thread_url);
                }
            }
            start = content.find(token, start+11);
        }
        return 0;
    }

    int itpub_Session::scanForum(const char* forum_url, url_list_t& thread_urls)
    {
        Trace log("itpub_Session::scanForum");
        itpub_Session_i::http_str_get_client_t http;
        std::string content;
        http.setOutput(content);
        http.setCookie(itpub_Session_i::COOKIE);
        http.request(forum_url);
        int maxPageNum = getMaxPageNum(content);
        for(int i=1; i<=maxPageNum; ++i)
        {
            std::string page_url;
            getForumUrl(forum_url, i, page_url);
            scanThreadsInForumPage(page_url, thread_urls);
            log.debug("Scan from page... %d/%d find threads %d\n", i, maxPageNum, thread_urls.size());
        }
        log.debug("Total theads: %d\n", thread_urls.size());
        return 0;
    }

    int itpub_Session::downloadThread(const char* thread_url, const char* dir)
    {
        enum{ERR_WRONG_REMOTE_FILE=-1};
        Trace log("itpub_Session::downloadThread"); 

        if(isSkipThread(thread_url))
            return -90;

        //---------------------------------------------
        // Scan attachments in thread
        url_list_t att_url_list;
        scanThread(thread_url, att_url_list);
        std::string thread_id = getThreadIdInternal(std::string(thread_url));
        int ret=0;
        for(url_list_t::iterator itr = att_url_list.begin(); itr!=att_url_list.end(); ++itr)
        {
            std::string att_url = (*itr);
            std::stringstream pss;
            pss << dir << "/" << thread_id;
            std::string path = pss.str();
            int f = create_dir(path.c_str());
            log.debug("Create dir %s result:%d\n", path.c_str(), f);
            if(f==-1)
                return -99;

            // bf::path path= dir;
            // path = path/thread_id;
            // if(!bf::exists(path)){
                // log.debug("Dir %s don't exist!\n", path.c_str());
                // bool f = bf::create_directory(path);
                // if(!f) return -99;
            // }
            log.debug("Downloading ... %s to %s\n", att_url.c_str(), path.c_str());
            int r = download(att_url.c_str(), path.c_str());
            if(r == CURLE_PARTIAL_FILE){
                ret = r;
            }
        }

        //---------------------------------------------
        // Some of the files in thread get wrong size,
        // Record it as threads.skip
        if(ret == CURLE_PARTIAL_FILE)
        {
            itpub_Config cfg;
            cfg.load("config.cfg");
            std::string threads_skip_str;
            cfg.get("threads.skip", threads_skip_str);
            threads_skip_str.append(thread_id).append(",");
            cfg.set("threads.skip", threads_skip_str);
            cfg.save("config.cfg");
            return ERR_WRONG_REMOTE_FILE;
        }

        //-----------------------------------------------
        // Threads finished 
        if(att_url_list.size()>0)
        {
            itpub_Config cfg;
            cfg.load("config.cfg");
            std::string threads_done_str;
            cfg.get("threads.done", threads_done_str);
            threads_done_str.append(thread_id).append(",");
            cfg.set("threads.done", threads_done_str);
            cfg.save("config.cfg");
        }
        return 0;
    }

    int itpub_Session::getThreadId(const std::string& thread_url, std::string ret)
    {
        ret = getThreadIdInternal(thread_url);
        return 0; 
    }
}
