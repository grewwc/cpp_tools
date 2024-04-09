#include "NormalUtils.hpp"
#include <ctime>
#include <iomanip>
#include <iostream>
#include <sstream>

using namespace std;
using namespace wwc;

void print_current_time() {
    time_t now = time(0);
    cout << now << " (s) " << endl;
}

int main(int argc, char *argv[]) {
    String s;
    ArgumentParser parser(argc, argv);
    parser.add_argument("-ts", ArgType::BOOL, "false", "show version");
    ParsedResult result = parser.parse();
    if (result.get_positional_args().size() == 1) {
        print_current_time();
        return 0;
    }
    std::variant<bool, std::string> val = result["ts"];
    if (std::get<bool>(val)) {
        print_current_time();
        return 0;
    }
    String ts = result.get_positional_arg(1);
    if (!ts.is_integer()) {
        vector<String> vec = ts.split(' ', true);
        std::istringstream ss{ts.c_str()};
        std::tm t;
        if (vec.size() == 1) {
            ss >> std::get_time(&t, "%Y-%m-%d");
            t.tm_hour = 0;
            t.tm_min = 0;
            t.tm_sec = 0;
        } else if (vec.size() == 2) {
            ss >> std::get_time(&t, "%Y-%m-%d %H:%M:%S");
        }
        cout << mktime(&t) << endl;
        return 0;
    }
    // argument is integer
    if (ts.size() != 10 && ts.size() != 13) {
        cout << String::format("invalid timestamp: %s\n", ts.c_str());
        return 0;
    }
    time_t now = (time_t)ts.to_long();
    if (ts.size() == 13) {
        now /= 1000L;
    }
    auto *timestamp = gmtime(&now);
    if (timestamp->tm_year > 1000) {
        String ts = result.get_positional_arg(0);
        now = (time_t)ts.to_long() / 1000;
        timestamp = gmtime(&now);
    }
    cout << String::format("%04d-%02d-%02d %02d:%02d:%02d", timestamp->tm_year + 1900, timestamp->tm_mon,
                           timestamp->tm_mday, timestamp->tm_hour, timestamp->tm_min, timestamp->tm_sec)
         << endl;
    return 0;
}