#pragma once

#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <functional>
#include <iostream>
#include <iterator>
#include <optional>
#include <regex>
#include <sstream>
#include <string>
#include <typeinfo>
#include <vector>
#include <iomanip>

#include "download_utils.hpp"

namespace wwc {

    class String : public std::string {
    public:
        String() = default;

        String(std::string &&s) noexcept : std::string{std::move(s)} {};

        String(const std::string &s) noexcept : std::string{s} {};

        String(const String &other) noexcept : std::string{other} {};

        String(String &&other) noexcept : std::string(std::move(static_cast<std::string>(other))) {}

        String(const char *s) noexcept : std::string{s} {}

        String &operator=(const String &other) noexcept {
            std::string::operator=(other);
            return *this;
        }

        String &operator=(String &&other) noexcept {
            std::string::operator=(std::move(static_cast<std::string>(other)));
            return *this;
        }

        String &operator=(std::string &&s) noexcept {
            std::string::operator=(std::move(s));
            return *this;
        }

        String &operator=(const std::string &s) noexcept {
            std::string::operator=(s);
            return *this;
        }

        String &operator=(const char *s) noexcept {
            std::string::operator=(s);
            return *this;
        }

        String &lstrip(char ch = ' ') noexcept {
            auto pos = find_first_not_of(ch);
            switch (pos) {
                case std::string::npos: break;
                default: this->erase(0, pos);
            }
            return *this;
        }

        String &ltrim(char ch = ' ') noexcept { return lstrip(ch); }

        String &rstrip(char ch = ' ') noexcept {
            std::string::size_type pos = find_last_not_of(ch);
            switch (pos) {
                case std::string::npos: break;
                default: this->erase(this->begin() + pos + 1, this->end());
            }
            return *this;
        }

        String &rtrim(char ch = ' ') noexcept { return rstrip(ch); }

        String &strip(char ch = ' ') noexcept {
            lstrip(ch);
            rstrip(ch);
            return *this;
        }

        String &trim(char ch = ' ') noexcept { return strip(ch); }

        String &strip_prefix(const std::string &prefix) noexcept {
            if (startsWith(prefix)) {
                this->erase(0, prefix.size());
            }
            return *this;
        }

        String &trim_prefix(const std::string &prefix) noexcept { return strip_prefix(prefix); }

        String lstrip_copy(char ch = ' ') const noexcept {
            String res{*this};
            res.lstrip(ch);
            return res;
        }

        String ltrim_copy(char ch = ' ') const noexcept { return lstrip_copy(ch); }

        String rstrip_copy(char ch = ' ') const noexcept {
            String res{*this};
            res.rstrip(ch);
            return res;
        }

        String rtrim_copy(char ch = ' ') const noexcept { return rstrip_copy(ch); }

        String strip_copy(char ch = ' ') const noexcept {
            String res{*this};
            res.strip(ch);
            return res;
        }

        String trim_copy(char ch = ' ') const noexcept { return strip_copy(ch); }

        static String from_file(String filename) {
            String result;
            result.from_file(filename);
            return result;
        }

        void from_file(const char *filename) {
            auto filename_str = expanduser(filename);
            const char *full_filename = filename_str.c_str();
            std::ifstream in{full_filename};
            if (!in) {
                perror("failed to open file");
                return;
            }
            this->clear();
            std::copy(std::istreambuf_iterator<char>(in), std::istreambuf_iterator<char>(), std::back_inserter(*this));
            in.close();
        }

        String plus(const String &other) const noexcept {
            if (!is_integer() || !other.is_integer()) {
                return "";
            }
            return string_plus(*this, other);
        }
        String operator-(const String &other) const noexcept {
            if (!is_integer() || !other.is_integer()) {
                return "";
            }
            return string_minus(*this, other);
        }
        String operator*(const String &other) const noexcept {
            if (!is_integer() || !other.is_integer()) {
                return "";
            }
            return string_mul(*this, other);
        }

        std::vector<String> split(char ch = ' ', bool keep_quote = false) const noexcept {
            std::vector<String> result;
            const auto N = size();
            String word;
            bool in_double_quote = false;
            bool in_single_quote = false;
            for (size_t i = 0; i < N; i++) {
                char cur_ch = (*this)[i];
                // in quote
                if (cur_ch == '"') {
                    if (keep_quote) {
                        if (in_double_quote) {
                            result.emplace_back(std::move(word));
                            word.clear();
                        } else if (in_single_quote) {
                            word += cur_ch;
                        }
                    }
                    in_double_quote = !in_double_quote;
                } else if (cur_ch == '\'') {
                    if (keep_quote) {
                        if (in_single_quote) {
                            result.emplace_back(std::move(word));
                            word.clear();
                        } else if (in_double_quote) {
                            word += cur_ch;
                        }
                    }
                    in_single_quote = !in_single_quote;
                } else if (ch == cur_ch) {
                    if ((!keep_quote || (!in_double_quote && !in_single_quote)) && !word.empty()) {
                        result.emplace_back(std::move(word));
                        word.clear();
                    } else if (in_double_quote || in_single_quote) {
                        word += cur_ch;
                    }
                } else {
                    word += cur_ch;
                }
            }
            if (!word.empty()) result.emplace_back(std::move(word));
            return result;
        }

        bool is_integer() const noexcept {
            if (size() == 0) {
                return false;
            }
            const char c1 = (*this)[0];
            if (c1 != '+' && c1 != '-' && !isdigit(c1)) {
                return false;
            }
            for (size_t i = 1; i < size(); i++) {
                const char ch = (*this)[i];
                if (!isdigit(ch)) {
                    return false;
                }
            }
            return true;
        }

        template <typename T>
        static String to_string(T val, const char *fmt = nullptr) noexcept {
            if (fmt == nullptr) {
                return std::to_string(val);
            }
            return format(fmt, std::forward<T>(val));
        }

        template <typename Container>
        static String join(const char *delim, Container &&data) {
            if (data.size() == 0) return String{""};
            std::ostringstream os;
            const auto delim_len = strlen(delim);
            for (auto &&each_str : std::forward<Container>(data)) {
                os << each_str << delim;
            }
            auto temp = os.str();
            // remove the last delim
            temp.erase(temp.end() - delim_len, temp.end());
            return String{std::move(temp)};
        }

        double to_double() const noexcept {
            char *end;
            auto res = std::strtod(c_str(), &end);
            if (errno == ERANGE) {
                fprintf(stderr, "range error (%s --> double)\n", c_str());
                errno = 0;
            }
            return res;
        }

        int to_int() const noexcept { return static_cast<int>(to_long()); }

        long to_long() const noexcept {
            char *end;
            auto res = std::strtod(c_str(), &end);
            if (errno == ERANGE) {
                fprintf(stderr, "range error (%s --> long)\n", c_str());
                errno = 0;
            }
            return res;
        }

        float to_float() const noexcept {
            char *end;
            auto res = std::strtof(c_str(), &end);
            if (errno == ERANGE) {
                fprintf(stderr, "range error (%s --> float)\n", c_str());
                errno = 0;
            }
            return res;
        }

        std::vector<long> find_all_long() const noexcept { return find_all_number<long>('l'); }

        std::vector<double> find_all_double() const noexcept { return find_all_number<double>('d'); }

        std::vector<std::size_t> find_all(const char *data) const noexcept {
            const auto DATA_LEN = strlen(data);
            const auto LEN = size();
            std::vector<size_t> res;
            size_t last_idx = 0LL;
            while (last_idx <= LEN - DATA_LEN) {
                auto pos = find(data, last_idx);
                if (pos == std::string::npos) break;
                res.emplace_back(pos);
                last_idx = pos + DATA_LEN;
            }
            return res;
        }

        std::vector<int> find_all_substr(const std::string &sub, std::size_t begin = 0) const noexcept {
            std::vector<int> result;
            int idx = -1;
            while (true) {
                idx = kmp(*this, sub, begin);
                if (idx == -1) {
                    break;
                }
                result.push_back(idx);
                begin = static_cast<std::size_t>(idx + sub.size());
            }
            return result;
        }

        bool is_match(String re_pattern) const noexcept {
            if (!re_pattern.startsWith("^")) {
                re_pattern = "^" + re_pattern;
            }
            if (!re_pattern.endsWith("$")) {
                re_pattern += '$';
            }
            std::regex re(re_pattern);
            std::sregex_iterator it(cbegin(), cend(), re);
            return it != std::sregex_iterator();
        }

        std::size_t count(char ch) const noexcept { return std::count(cbegin(), cend(), ch); }

        String toLower_copy() const noexcept {
            String res = *this;
            res.toLower();
            return res;
        }
        String &toLower() noexcept {
            const size_t SIZE = (*this).size();
            for (size_t i = 0; i < SIZE; ++i) {
                char temp = (*this)[i];
                if (isalpha(temp)) {
                    (*this)[i] = tolower(temp);
                    continue;
                }
            }
            return *this;
        }

        String toUpper_copy() const noexcept {
            String res = *this;
            res.toUpper();
            return res;
        }
        String &toUpper() noexcept {
            const size_t SIZE = this->size();
            char temp;
            for (size_t i = 0; i < SIZE; ++i) {
                temp = (*this)[i];
                if (isalpha(temp)) (*this)[i] = toupper(temp);
            }
            return *this;
        }

        String toCap_copy() const noexcept {
            String res{*this};
            return res.toCap();
        }
        String &toCap() noexcept {
            bool flag = true;
            const size_t SIZE = (*this).size();
            char temp;
            for (size_t i = 0; i < SIZE; ++i) {
                temp = (*this)[i];
                while (isspace(temp)) {
                    flag = true;
                    temp = (*this)[++i];
                }

                while (!isalpha(temp) && !isspace(temp)) temp = (*this)[++i];

                if (flag && isalpha(temp)) {
                    (*this)[i] = toupper(temp);
                    flag = false;
                }
            }
            return *this;
        }

        bool startsWith(const std::string &begin) const noexcept {
            size_t begin_size = begin.size();
            return std::string::find(begin) == 0;
        }

        bool endsWith(const std::string &end) const noexcept {
            const size_t END_SIZE = end.size();
            const size_t SIZE = size();
            if (END_SIZE > SIZE) return false;
            const size_t DIFF = SIZE - END_SIZE;
            const String &this_obj = *this;
            for (size_t i = 0; i < END_SIZE; i++) {
                if (this_obj[DIFF + i] != end[i]) return false;
            }
            return true;
        }

        String url_encode(const String &value) const {
            std::ostringstream escaped;
            escaped.fill('0');
            escaped << std::hex;

            for (char c : value) {
                // 不需要转义的字符直接添加
                if (isalnum(c) || c == '-' || c == '_' || c == '.' || c == '~') {
                    escaped << c;
                } else {
                    // 需要转义的字符转换为 %XX 形式
                    escaped << '%' << std::setw(2) << int((unsigned char)c);
                }
            }

            return escaped.str();
        }

        String url_decode(const String &value) const {
            std::ostringstream decoded;
            for (size_t i = 0; i < value.length(); ++i) {
                if (value[i] == '%' && i + 2 < value.length()) {
                    // 解析 %XX 形式的编码
                    std::string hex_str = value.substr(i + 1, 2);
                    char decoded_char = static_cast<char>(std::strtol(hex_str.c_str(), nullptr, 16));
                    decoded << decoded_char;
                    i += 2;  // 跳过两个字符
                } else {
                    // 直接添加未编码的字符
                    decoded << value[i];
                }
            }
            return decoded.str();
        }

        // bool startsWith(const char *begin) const noexcept;
        // bool endsWith(const char *end) const noexcept;

        bool contains(const char *sub) const noexcept { return strstr(data(), sub); }

        bool contains(const std::string &sub) const noexcept { return contains(sub.data()); }

        String operator*(const size_t N) const noexcept {
            String res;
            const size_t SIZE = this->size();
            res.resize(N * SIZE);
            for (size_t i = 0; i < N; i++) {
                std::copy(this->cbegin(), this->cend(), res.begin() + i * SIZE);
            }
            return res;
        }
        friend String operator*(const size_t N, const String &ws) { return ws * N; }
        void operator*=(const size_t N) noexcept { *this = (*this) * N; }

        String replace_substr(const char *old, const char *new_) const noexcept {
            String copy = *this;
            auto positions = copy.find_all(old);
            const int old_len = strlen(old);
            const int new_len = strlen(new_);
            for (int i = 0; i < positions.size(); i++) {
                int position = positions[i];
                // std::cout << "here " << position << std::endl;
                position += i * (new_len - old_len);
                copy.replace(copy.begin() + position, copy.begin() + position + old_len, new_);
            }
            return copy;
        }

        String replace_char(char old, char new_) const noexcept {
            return replace_char_fn_copy([old, new_](char ch) { return ch == old ? new_ : ch; });
        }

        template <typename T>
        String &operator+=(T &&val) {
            using raw_type = std::decay_t<decltype(val)>;
            constexpr bool is_char = std::is_same_v<char, raw_type>;
            constexpr bool is_integer = std::is_integral_v<raw_type>;
            constexpr bool is_floating_point = std::is_floating_point_v<raw_type>;
            if constexpr (is_floating_point && !is_char) {
                std::string temp_result = std::to_string(val);
                // remove trailing 0
                temp_result.erase(temp_result.find_last_not_of('0') + 1, std::string::npos);
                std::string::operator+=(std::move(temp_result));
            } else if constexpr (is_integer && !is_char) {
                std::string::operator+=(std::to_string(val));
            } else {
                std::string::operator+=(val);
            }

            return *this;
        }

        std::optional<String> download_url() const {
            String result;
            if (!wwc::download_url(this->c_str(), result)) {
                return {};
            }
            return result;
        }

        bool write_to_file(const char *filename, const char *mode = "w") const {
            String temp = expanduser(filename);
            FILE *f = fopen(temp.c_str(), mode);
            if (f == nullptr) {
                return false;
            }
            fwrite(this->c_str(), this->size(), 1, f);
            fclose(f);
            return true;
        }

        template <typename... Args>
        static String format(const char *fmt, Args... args) {
            const auto sz = snprintf(nullptr, 0, fmt, args...);
            std::vector<char> buf(sz + 1);
            snprintf(buf.data(), sz + 1, fmt, std::forward<Args>(args)...);
            return String(buf.data());
        }

    private:
        template <typename T>
        std::vector<T> find_all_number(char type) const noexcept {
            const auto &fn = [](char ch) {
                if (!isdigit(ch) && ch != '.' && ch != 'e' && ch != '+' && ch != '-') return ' ';
                return ch;
            };

            String spaceSaperated = replace_char_fn_copy(fn);

            std::vector<T> res;
            T i;
            char *end;
            const char *beg = spaceSaperated.c_str();
            while (true) {
                switch (type) {
                    case 'l': i = strtol(beg, &end, 10); break;
                    case 'd': i = strtod(beg, &end); break;
                    default: printf("unknow type %c\n", type); goto end;
                }
                if (errno == ERANGE) {
                    fprintf(stderr, "range error\n");
                    errno = 0;
                }
                if (beg == end) break;
                res.push_back(i);
                beg = end;
            }
        end:
            return res;
        }

        String replace_char_fn_copy(const std::function<char(char)> &func) const noexcept {
            String res;
            res.resize(this->size());
            std::transform(cbegin(), cend(), res.begin(), func);
            return res;
        }

        static std::vector<int> get_kmp_prefix_array(const std::string &t) noexcept {
            std::vector<int> prefix(t.size(), -1);
            for (int j = 2; j < t.size(); j++) {
                int k = prefix[j - 1];
                if (k == -1) {
                    prefix[j] = (t[0] == t[j - 1]) ? 1 : -1;
                } else {
                    if (t[k] == t[j - 1]) {
                        prefix[j] = k + 1;
                    } else {
                        int kk = k;
                        while (kk != -1) {
                            kk = prefix[kk];
                            if (kk == -1) {
                                prefix[j] = (t[0] == t[j - 1]) ? 1 : -1;
                            } else if (t[kk] == t[j - 1]) {
                                prefix[j] = kk + 1;
                                break;
                            }
                        }
                    }
                }
            }
            return prefix;
        }

        static int kmp(const std::string &s, const std::string &sub, std::size_t begin) noexcept {
            std::vector<int> next = get_kmp_prefix_array(sub);
            std::size_t i = begin;
            int j = 0;
            while (i < s.size() && j < sub.size()) {
                if (s[i] == sub[j]) {
                    i++, j++;
                } else {
                    j = next[j];
                    if (j == -1) {
                        i++;
                        j = 0;
                    }
                }
            }
            return j == sub.size() ? i - sub.size() : -1;
        }

        static std::string expanduser(const char *path) noexcept {
            const char *home = std::getenv("HOME");
            if (home == nullptr) {
                return path;
            }
            String s{path};
            return s.replace_substr("~", home);
        }

        static String string_mul(const String &left, const String &right) {
            String s1 = left.lstrip_copy('+');
            String s2 = right.lstrip_copy('+');
            if (s1 == "0" || s2 == "0") {
                return "0";
            }
            // check sign
            bool minus = (left[0] == '-' && right[0] != '-') || (left[0] != '-' && right[0] == '-');
            s1 = s1.lstrip('-').lstrip('0');
            s2 = s2.lstrip('-').lstrip('0');
            const auto l1 = s1.size();
            const auto l2 = s2.size();
            if (l1 == 0 || l2 == 0) {
                return "";
            }
            std::vector<int> result(l1 + l2 + 1, 0);
            std::vector<int> overflow(l1 + l2 + 1, 0);
            for (size_t j = 0; j < l2; j++) {
                for (size_t i = 0; i < l1; i++) {
                    int val = (s1[l1 - 1 - i] - '0') * (s2[l2 - 1 - j] - '0') + overflow[i + j] + result[i + j];
                    overflow[i + j] = 0;
                    if (val >= 10) {
                        overflow[i + j + 1] += val / 10;
                        if (overflow[i + j + 1] >= 10) {
                            overflow[i + j + 2] += overflow[i + j + 1] / 10;
                            overflow[i + j + 1] %= 10;
                        }
                        val %= 10;
                    }
                    result[i + j] = val;
                }
            }
            for (std::size_t i = 0; i < result.size() - 1; i++) {
                result[i] += overflow[i];
                if (result[i] >= 10) {
                    result[i + 1] += result[i] / 10;
                    result[i] %= 10;
                }
            }
            std::reverse(result.begin(), result.end());

            String s;
            s.reserve(result.size());
            std::transform(result.cbegin(), result.cend(), std::back_inserter(s),
                           [](int val) -> char { return (char)(val + '0'); });
            s.lstrip('0');
            if (minus) {
                return String("-") + s;
            }
            return s;
        }

        static String string_plus(const String &left, const String &right) {
            String s1 = left.lstrip_copy('+');
            String s2 = right.lstrip_copy('+');

            // both minus
            if (s1[0] == '-' && s2[0] == '-') {
                return String("-") + string_plus(s1.substr(1, s1.size() - 1), s2.substr(1, s2.size() - 1));
            }
            // one plus, one minus
            if (s1[0] == '-' && s2[0] != '-') {
                return String("-") + string_minus(s1.substr(1, s1.size() - 1), s2);
            }
            if (s1[0] != '-' && s2[0] == '-') {
                return string_minus(s1, s2.substr(1, s2.size() - 1));
            }
            s1 = s1.lstrip('0');
            s2 = s2.lstrip('0');
            if (s1.size() < s2.size()) {
                std::swap(s1, s2);
            }
            const auto l1 = s1.size();
            const auto l2 = s2.size();
            if (l1 == 0 || l2 == 0) {
                return "";
            }
            int overflow = 0;
            std::vector<int> result(l1 + 1, 0);
            for (std::size_t i = 0; i < l1; i++) {
                const int right_val = (i >= l2) ? 0 : (s2[l2 - 1 - i] - '0');
                int val = (s1[l1 - 1 - i] - '0') + right_val + overflow;
                overflow = 0;
                if (val >= 10) {
                    overflow = val / 10;
                    val %= 10;
                }
                result[i] = val;
            }
            result[l1] = overflow;
#if 0
            for(auto e : result) {
                std::cout << e << ",";
            }
            std::cout << std::endl;
#endif
            std::reverse(result.begin(), result.end());
            String s;
            s.reserve(l1 + 1);
            std::transform(result.cbegin(), result.cend(), std::back_inserter(s),
                           [](int val) { return (char)(val + '0'); });
            if (s.size() == 1 && s[0] == '0') {
                return s;
            }
            return s.lstrip('0');
        }

        static String string_minus(const String &left, const String &right) {
            int swapped = 0;
            String s1 = left.lstrip_copy('+');
            String s2 = right.lstrip_copy('+');
            // left > 0, right < 0
            if (left[0] != '-' && right[0] == '-') {
                return string_plus(left, right.substr(1, right.size() - 1));
            }
            // left < 0, right > 0
            if (left[0] == '-' && right[0] != '-') {
                return String("-") + string_plus(left.substr(1, left.size() - 1), right);
            }
            // both minus
            if (s1[0] == '-' && s2[0] == '-') {
                swapped++;
            }
            s1 = s1.lstrip('-').lstrip('0');
            s2 = s2.lstrip('-').lstrip('0');
            if (s1.size() < s2.size()) {
                std::swap(s1, s2);
                swapped++;
            }
            if (s1.size() == s2.size() && s1 < s2) {
                std::swap(s1, s2);
                swapped++;
            }
            const auto l1 = s1.size();
            const auto l2 = s2.size();
            if (l1 == 0 || l2 == 0) {
                return "0";
            }
            std::vector<int> result(l1, 0);
            int owed = 0;
            for (std::size_t i = 0; i < l1; i++) {
                const int right_val = (i >= l2) ? 0 : s2[l2 - 1 - i] - '0';
                const int left_val = s1[l1 - 1 - i] - owed - '0';
                int val = left_val - right_val;
                if (val < 0) {
                    owed = 1;
                    val += 10;
                } else {
                    owed = 0;
                }
                result[i] = val;
            }
#if 0
            for (auto e : result) {
                std::cout << e << ",";
            }
            std::cout << std::endl;
#endif
            String s;
            s.resize(result.size());
            std::reverse(result.begin(), result.end());
            std::transform(result.cbegin(), result.cend(), s.begin(), [](int val) { return (char)(val + '0'); });
            if (s.size() == 1 && s[0] == '0') {
                return s;
            }
            s = s.lstrip('0');
            if (s.empty()) {
                return "0";
            }
            if (swapped % 2 == 1) {
                return String("-") + s;
            }
            return s;
        }
    };
}  // namespace wwc



namespace std
{
    template<>
    struct hash<wwc::String> 
    {
      public:
        std::size_t operator()(const wwc::String& s) const noexcept {
            return hasher(s);
        }
      private:
        const hash<std::string> hasher = std::hash<std::string>{};
        
    };
}
