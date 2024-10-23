
#include "NormalUtils.hpp"

using namespace wwc;

const char* default_n = "5";

ParsedResult parse(int argc, char* argv[]) {
    ArgumentParser parser{argc, argv};
    parser.add_argument("-n", ArgType::STRING, default_n, "Output file");
    ParsedResult parsed_result = parser.parse();
    return parsed_result;
}

int get_n(int argc, char* argv[]) {
    ParsedResult parsed_result = parse(argc, argv);
    std::variant<bool, std::string> var_n = parsed_result.get_val("n").value_or(default_n);
    int n = std::make_shared<String>(std::get<std::string>(var_n))->to_int();
    int default_val = String(default_n).to_int();
    if (default_val == n && argc == 2) {
        String first_arg = argv[1];
        first_arg.lstrip('-');
        if (first_arg.is_integer()) {
            return first_arg.to_int();
        }
    }
    return n;
}

int main(int argc, char* argv[]) {
    int n = get_n(argc, argv);
    String output = run_cmd("git log --oneline --format=\"%h %ad %s\" --date=short");
    Stream<std::vector, String>::from(output.split('\n'))
        .filter([](String line) { return !line.contains("Merge"); })
        .first(n)
        .for_each([](const String& s) {std::cout << s << std::endl;});
    
    return 0;
}