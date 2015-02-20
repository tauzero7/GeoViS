#ifndef  GVS_LOG_H
#define  GVS_LOG_H

#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <cstdarg>

class GvsLog {
public:
    static  GvsLog&  instance();
    ~GvsLog();

    void  setLogFile   ( std::string fileName, int logLevel = 0 );
    void  writeErrMsg  ( std::string file, int lineNum, std::string msg );

    void  printf       ( const char* fmt, ... );
    void  printf       ( int level, const char* fmt, ... );
    void  operator<<   ( const std::string msg );

    int   level();
    FILE* ptr();

private:
    GvsLog();
    GvsLog( const GvsLog& );             // copy constructor prevents that another instance can be created
    GvsLog &operator = (const GvsLog &); // prevents further instances by copying


private:
    int     logLevel;
    FILE*   fptr;
};

#endif // GVS_LOG_H
