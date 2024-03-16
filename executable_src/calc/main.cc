#include <iostream>

#include "NormalUtils.hpp"

using namespace std;
using namespace wwc;

int main(int argc, char* argv[]) {
    if (argc != 2) {
        cout << "c 2**2" << endl;
        return 0;
    }
    String s = String::format("python -c \"print(%s)\"", argv[1]);
    s = run_cmd(s.c_str());
    cout << s.trim('\n') << endl;
}