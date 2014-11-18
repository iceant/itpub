#include "itpub_logger.hpp"
#include <ctime>

using namespace PointCX;
using namespace std;

bool Trace::isTraceActive = false;
int addOne(int x)
{
    Trace t("addOne");
    return x+1;
}
int main(int argc, char** argv)
{
    time_t start;
    time(&start);
    for(int i=0; i< 1000000; i++){
        addOne(i);
    }
    time_t end;
    time(&end);
    std::cout << end - start << std::endl;

    return 0;
}
