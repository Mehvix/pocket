#ifndef POCKET_HELPER_H
#define POCKET_HELPER_H

#include <ctime>
#include <string>
#include <sstream>
#include <fstream>

using namespace std;

namespace Helper {
    template <class T>
    string toString(const T &);

    struct DateTime {
        int day, month, year, hour, min, sec;

        DateTime() {
            time_t ms;
            time(&ms);

            struct tm *info = localtime(&ms);

            day = info->tm_mday;
            month = info -> tm_mon + 1;  // starts at 0
            year = 1900 + info -> tm_year;  // unix starts 1900
            hour = info -> tm_hour;
            min = info -> tm_min;
            sec = info -> tm_sec;
        }

        DateTime(int day, int month, int year, int hour, int min, int sec) : day(day), month(month), year(year), hour(hour), min(min), sec(sec) {}
        DateTime(int day, int month, int year) : day(day), month(month), year(year), hour(0), min(0), sec(0) {}


        DateTime now() const {
            return DateTime();
        }

        string getDate() const {
            return  string(day < 10 ? "0" : "") + toString(day) + string(month < 10 ? ".0" : ".") + toString(month) + "." + toString(year);
        }

        string getTime(const string &separator = ":") const{
            return  string(hour < 10 ? "0" : "") + toString(hour) + separator + string(min < 10 ? "0" : "") + toString(min) + separator + string( sec < 10 ? separator : "") + toString(sec);
        }

        string getDateTime(const string &separator = ":") const {
            return getDate() + " " + getTime(separator);
        }
    };

    template <class T>
    string toString(const T &e){
        ostringstream s;
        s << e;
        return s.str();
    }

    void writeLog(const string &s){
        ofstream file("log.txt", ios::app);
        file << "[" << Helper::DateTime().getDateTime() << "]" << "\n" << s << endl << "\n";
        file.close();
    }
}

#endif //POCKET_HELPER_H
