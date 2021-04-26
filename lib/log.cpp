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
    get_logfile();
    string filename = LOGFOLDER + LOGNAME + ".txt";
    ofstream log;
    log.open(filename);
    log << s;
    log.close();
}