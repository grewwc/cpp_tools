#include "fileutils.hpp"

#include <fstream>
#include <iostream>
using namespace std;

namespace wwc {
    string readTextFromFile(const char* filename) {
        ifstream in{filename};
        if (!in.is_open()) {
            perror("readAllFromFile error:");
            return "";
        }
        in.seekg(0, ios::end);
        auto sz = in.tellg();
        in.seekg(0, ios::beg);
        char* buf = new char[sz];
        in.read(buf, sz);
        string result{buf};
        delete[] buf;
        in.close();
        return result;
    }

    bool writeTextToFile(const char* filename, const char* content) {
        ofstream out{filename};
        if (!out.is_open()) {
            perror("Cannot open file to write: ");
            return false;
        }
        out << content;
        out.close();
        return true;
    }
}  // namespace wwc