#ifndef ITPUB_CONFIG_HEAD
#define ITPUB_CONFIG_HEAD

#ifndef ITPUB_STD_STRING_H
#define ITPUB_STD_STRING_H
#include <string>
#endif //ITPUB_STD_STRING_H

#ifndef ITPUB_STD_VECTOR_H
#define ITPUB_STD_VECTOR_H
#include <vector>
#endif //ITPUB_STD_VECTOR_H

#ifndef ITPUB_STD_IOSFWD_H
#define ITPUB_STD_IOSFWD_H
#include <iosfwd>
#endif //ITPUB_STD_IOSFWD_H

namespace PointCX
{
    class itpub_Config_i;
    class itpub_Config
    {
        private:
            itpub_Config(const itpub_Config&);
            itpub_Config& operator=(const itpub_Config&);
            itpub_Config_i *d_this;

        public:
            itpub_Config();
            ~itpub_Config();

            int set(const std::string& key, const std::string& value);

            int get(const std::string& key, std::string& ret)const;

            int save(const std::string& file);

            int load(const std::string& file);

            int keys(std::vector<std::string>& ret)const;

            friend std::ostream& operator<<(std::ostream&, const itpub_Config&);
            friend std::istream& operator>>(std::istream&, itpub_Config&);
    };
}
#endif //ITPUB_CONFIG_HEAD
