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