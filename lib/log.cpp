#include <iostream>
#include <fstream>
#include <ctime>
#include <chrono>
#include "log.hpp"
using namespace std;

const string LOGFOLDER = "log/";
string LOGNAME;

void get_logfile() {
    auto time_now = chrono::system_clock::to_time_t(chrono::system_clock::now());
    LOGNAME = ctime(&time_now);
}

void write_log(string s) {
    string filename = LOGFOLDER + LOGNAME + ".txt";
    ofstream log (filename, ios::ate | ios::app);
    log << s;
    log.close();
}

void write_log(vector<string> v) {
    string filename = LOGFOLDER + LOGNAME + ".txt";
    ofstream log (filename, ios::ate | ios::app);
    for (auto s : v) {
        log << s << "\n";
    }
    log.close();
}

string find_time(int seconds) {
    int day, hour, minutes;
    const int SECONDS_IN_MINUTE = 60;
    const int SECONDS_IN_HOUR = 60*SECONDS_IN_MINUTE;
    const int SECONDS_IN_DAY = 24*SECONDS_IN_HOUR;
    if (seconds < SECONDS_IN_DAY) {
        day = 1;
    }
    else {
        day = 2;
        seconds -= SECONDS_IN_DAY;
    }

    hour = seconds / SECONDS_IN_HOUR;
    seconds %= SECONDS_IN_HOUR;

    minutes = seconds / SECONDS_IN_MINUTE;
    string s = to_string(day)+':'+to_string(hour)+':'+to_string(minutes);
    if (minutes == 0) {
        s += '0';
    }

    return s;
}