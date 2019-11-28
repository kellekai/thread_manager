#include <iostream>
#include "FTthreadManager.h"
#include <unistd.h>

void foo( std::string str )
{
    std::cout << str << std::endl; 
}

int main() 
{
    FTthreadManager tm;
    tm.submit( foo, std::string("the") );
    tm.submit( foo, std::string("quick") );
    tm.submit( foo, std::string("brown") );
    tm.submit( foo, std::string("fox") );
    tm.submit( foo, std::string("jumps") );
    tm.submit( foo, std::string("over") );
    tm.submit( foo, std::string("the") );
    tm.submit( foo, std::string("lazy") );
    tm.submit( foo, std::string("dog") );
    tm.synchronize();
}
