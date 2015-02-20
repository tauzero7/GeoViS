
#include "Utils.h"

#include <cstring>
#include <cstdio>
#include <cstring>
#include <algorithm>
#include <dirent.h>
#include <unistd.h> 
#include <sys/param.h>

bool  GetFileNames( const std::string path, const std::vector<std::string> extensions,
                    std::vector<std::string> &filePathNames ) {

    std::string currProgPath = GetProgPath();

    filePathNames.clear();
    std::string basePath = path;

    DIR *d = NULL;
    struct dirent *currDir;
    basePath += "/";

    if (chdir(basePath.c_str())!=0) {
        fprintf(stderr,"Folder %s is not found.\n",basePath.c_str());
        return false;
    }

    if ((d = opendir(".")) == NULL) {
        fprintf(stderr,"Cannot open dir.\n");
        return false;
    }

    while ((currDir = readdir(d))) {
        if (strcmp(currDir->d_name,".")==0 ||
                strcmp(currDir->d_name,"..")==0 ||
                strncmp(currDir->d_name,".",1)==0) {
            continue;
        }
        if (currDir->d_type != DT_DIR) {
            std::string fn = std::string(currDir->d_name);
            for(unsigned int i=0; i<extensions.size(); i++) {
                if (extensions[i].length() <= fn.length()) 
                {
                    size_t pos = fn.compare(fn.length()-extensions[i].length(),extensions[i].length(),extensions[i]);
                    if (pos==0) {    
                        //std::cerr << (currDir->d_name) << std::endl;
                        filePathNames.push_back(basePath + std::string(currDir->d_name));
                    }
                }
            }
        }
    }
    closedir(d);
    std::sort(filePathNames.begin(),filePathNames.end());   

    if (chdir(currProgPath.c_str())==0) {
        return true;
    }
    return false;
}


bool  FileEndsWith( const std::string filename, const std::string extension ) {
    size_t fileLen = filename.length();
    size_t extLen  = extension.length();
    if (extLen>=fileLen) {
        return false;
    }
    if (filename.compare(fileLen-extLen,extLen,extension)==0) {
        return true;
    }
    return false;
}


int   HaveNumInFilename( const std::string filename ) {
    size_t pos1 = filename.find_last_of("_");
    size_t pos2 = filename.find_last_of(".");
    if (pos1>=pos2 || pos1==std::string::npos || pos2==std::string::npos) {
        return -1;
    }
    int num = atoi(filename.substr(pos1+1,pos2-pos1).c_str());
//std::cerr << num << std::endl;
    return num;
}


std::string GetProgPath () {
    std::string path;
    pid_t pid = getpid();
    char buf[20] = {0};
    sprintf(buf,"%d",pid);
    std::string _link = "/proc/";
    _link.append(buf);
    _link.append("/exe");
    char proc[512];
    int ch = readlink(_link.c_str(),proc,512);
    if (ch != 1) {
        proc[ch] = 0;
        path = proc;
        std::string::size_type t = path.find_last_of("/");
        path = path.substr(0,t);
        return path + std::string("/");
    }
    return std::string();
}

