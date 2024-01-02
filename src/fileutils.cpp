#include "fileutils.hpp"

#include <dirent.h>
#include <sys/stat.h>

#include <fstream>
#include <iostream>

#include "String.hpp"
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

    std::string expanduser(const char* path) {
        const char* home = std::getenv("HOME");
        if (home == nullptr) {
            return path;
        }
        String s{path};
        return s.replace_substr("~", home);
    }

    bool is_dir(const char* path) {
        const auto s = expanduser(path);
        path = s.c_str();
        struct stat buf;
        if (stat(path, &buf) != 0) {
            perror("error");
            return false;
        }
        return S_ISDIR(buf.st_mode);
    }

    bool is_regular(const char* path) {
        path = expanduser(path).c_str();
        struct stat buf;
        if (stat(path, &buf) != 0) {
            return false;
        }
        return S_ISREG(buf.st_mode);
    }

    bool exists(const char* path) {
        path = expanduser(path).c_str();
        struct stat buf;
        return stat(path, &buf) != 0;
    }

    std::vector<std::string> ls_dir(const char* dir_name) {
        dir_name = expanduser(dir_name).c_str();
        DIR* dir = opendir(dir_name);
        if (dir == nullptr) {
            perror("Cannot open dir");
            return {};
        }
        dirent* ent = nullptr;
        std::vector<std::string> result;
        while ((ent = readdir(dir)) != nullptr) {
            result.emplace_back(ent->d_name);
        }
        closedir(dir);
        return result;
    }
}  // namespace wwc
