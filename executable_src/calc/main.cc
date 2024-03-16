#include <iostream>

#include "NormalUtils.hpp"

using namespace std;
using namespace wwc;

int main(int argc, char* argv[]) {
    if (argc != 2) {
        cout << "c 2**2" << endl;
        return 0;
    }
    String cmd = "python3";
    String s = run_cmd("which python3");
    if (s.trim().empty()) {
        cmd = "python";
    }
    s = String::format("%s -c \"print(%s)\"", cmd.c_str(), argv[1]);
    s = run_cmd(s.c_str());
    cout << s.trim('\n') << endl;
}