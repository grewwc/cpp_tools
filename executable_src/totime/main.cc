#include "NormalUtils.hpp"
#include <iostream>

using namespace std;
using namespace wwc;

int main(int argc, char *argv[]) {
    String s;
    for (int i = 0; i < argc; i++) {
        s += argv[i];
        s += " ";
    }
    ArgumentParser parser(s.c_str());
    parser.add_argument("-ts", ArgType::BOOL, "false", "show version");
    ParsedResult result = parser.parse();
    std::variant<bool, std::string> val = result["ts"];
    if (std::get<bool>(val)) {
        
    }
}