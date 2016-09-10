
#include "GvsLog.h"

GvsLog :: GvsLog() {
    this->logLevel = 0;
    this->fptr = stderr;
}

GvsLog :: ~GvsLog() {
    if (fptr!=NULL && fptr!=stderr && fptr!=stdout) {
        fclose(fptr);
    }
}

GvsLog& GvsLog :: instance() {
    static GvsLog theInstance;
    return theInstance;
}


void GvsLog :: setLogFile( std::string fileName, int logLevel ) {
    this->logLevel = logLevel;
#ifdef _WIN32
	fopen_s(&this->fptr, fileName.c_str(), "w");
#else
    this->fptr = fopen(fileName.c_str(),"w");
#endif
    if (fptr==NULL) {
        fprintf(stderr,"Cannot open logfile for output - exiting now.\n");
        exit(1);
    }
    fprintf(fptr,"Start logging...\n");
    fflush(fptr);
}

void GvsLog :: writeErrMsg( std::string file, int lineNum, std::string msg ) {
    size_t pos = file.find_last_of("/");
    size_t l   = file.length();
    std::string cfile = file.substr(pos+1,l-pos-1);
}

void GvsLog::printf( int level, const char* fmt, ... ) {
    if (level<=logLevel) {
        va_list ap;
        char text[1024];
        va_start(ap,fmt);
#ifdef _WIN32
		vsprintf_s(text, fmt, ap);
#else
        vsprintf(text,fmt,ap);
#endif
        va_end(ap);

        fprintf(fptr,"%s\n",text);
        fflush(fptr);

        if (ferror(fptr)) {
            fprintf(stderr,"Error writing to file.\n");
        }
    }
}

void GvsLog::printf( const char* fmt, ... ) {
    if (fmt==NULL) {
        return;
    }

    va_list ap;
    char text[1024];
    va_start(ap,fmt);
#ifdef _WIN32
	vsprintf_s(text, fmt, ap);
#else
    vsprintf(text,fmt,ap);
#endif
    va_end(ap);

    fprintf(fptr,"%s\n",text);
    fflush(fptr);

    if (ferror(fptr)) {
        fprintf(stderr,"Error writing to file.\n");
    }
}

void GvsLog :: operator<<( const std::string msg ) {
    fprintf(fptr,"%s\n",msg.c_str());
    fflush(fptr);
}

int GvsLog::level() {
    return this->logLevel;
}

FILE* GvsLog::ptr() {
    return this->fptr;
}
