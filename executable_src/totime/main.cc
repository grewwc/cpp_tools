#include "NormalUtils.hpp"
#include <ctime>
#include <iostream>

using namespace std;
using namespace wwc;

void print_current_time() {
    time_t now = time(0);
    cout << now << " (s) " << endl;
}

int main(int argc, char *argv[]) {
    String s;
    for (int i = 1; i < argc; i++) {
        s += argv[i];
        s += " ";
    }
    ArgumentParser parser(s.c_str());
    parser.add_argument("-ts", ArgType::BOOL, "false", "show version");
    ParsedResult result = parser.parse();
    if (result.get_positional_args().empty()) {
        print_current_time();
        return 0;
    }
    std::variant<bool, std::string> val = result["ts"];
    if (std::get<bool>(val)) {
        print_current_time();
        return 0;
    }
    String ts = result.get_positional_arg(0);
    time_t now = (time_t)ts.to_long();
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