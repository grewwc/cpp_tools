#pragma once

#include <algorithm>
#include <functional>
#include <string>
#include <vector>

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <sstream>

#include <iostream>

namespace wwc
{
    class String : public std::string
    {
    public:
        String() = default;

        String(std::string &&s) noexcept : std::string{std::move(s)} {};

        String(const std::string &s) noexcept : std::string{s} {};

        String(const String &other) noexcept : std::string{other} {};

        String(String &&other) noexcept : std::string((std::string &&)std::move(other)) {}

        String(const char *s) noexcept : std::string{s} {}

        String &operator=(const String &other) noexcept
        {
            std::string::operator=(other);
            return *this;
        }

        String &operator=(String &&other) noexcept
        {
            std::string::operator=((std::string &&)move(other));
            return *this;
        }

        String &operator=(std::string &&s) noexcept
        {
            std::string::operator=(move(s));
            return *this;
        }

        String &operator=(const std::string &s) noexcept
        {
            std::string::operator=(s);
            return *this;
        }

        String &operator=(const char *s) noexcept
        {
            std::string::operator=(s);
            return *this;
        }

        String &lstrip(char ch = ' ') noexcept
        {
            auto pos = find_first_not_of(ch);
            switch (pos)
            {
            case std::string::npos: break;
            default: this->erase(0, pos);
            }
            return *this;
        }

        String &rstrip(char ch = ' ') noexcept
        {
            std::string::size_type pos = find_last_not_of(ch);
            switch (pos)
            {
            case std::string::npos: break;
            default: this->erase(this->begin() + pos + 1, this->end());
            }
            return *this;
        }

        String &strip(char ch = ' ') noexcept
        {
            lstrip(ch);
            rstrip(ch);
            return *this;
        }

        String lstrip_copy(char ch = ' ') const noexcept
        {
            String res{*this};
            res.lstrip(ch);
            return res;
        }

        String rstrip_copy(char ch = ' ') const noexcept
        {
            String res{*this};
            res.rstrip(ch);
            return res;
        }

        String strip_copy(char ch = ' ') const noexcept
        {
            String res{*this};
            res.strip(ch);
            return res;
        }

        std::vector<String> split(char ch = ' ') const noexcept
        {
            std::vector<String> result;
            const auto N = size();
            String word;
            for (int i = 0; i < N; i++)
            {
                char cur_ch = (*this)[i];
                if (ch == cur_ch)
                {
                    if (!word.empty())
                    {
                        result.emplace_back(move(word));
                        word.clear();
                    }
                }
                else
                {
                    word += cur_ch;
                }
            }
            if (!word.empty()) result.emplace_back(move(word));
            return result;
        }

        template <typename Container>
        static String join(const char *delim, Container &&data)
        {
            if (data.size() == 0) return String{""};
            std::ostringstream os;
            const auto delim_len = strlen(delim);
            for (auto &&each_str : std::forward<Container>(data))
            {
                os << each_str << delim;
            }
            auto temp = os.str();
            // remove the last delim
            temp.erase(temp.end() - delim_len, temp.end());
            return String{std::move(temp)};
        }

        double to_double() const noexcept
        {
            char *end;
            auto res = std::strtod(c_str(), &end);
            if (errno == ERANGE)
            {
                fprintf(stderr, "range error (%s --> double)\n", c_str());
                errno = 0;
            }
            return res;
        }
        int to_int() const noexcept { return static_cast<int>(to_long()); }
        long to_long() const noexcept
        {
            char *end;
            auto res = std::strtod(c_str(), &end);
            if (errno == ERANGE)
            {
                fprintf(stderr, "range error (%s --> long)\n", c_str());
                errno = 0;
            }
            return res;
        }
        float to_float() const noexcept
        {
            char *end;
            auto res = std::strtof(c_str(), &end);
            if (errno == ERANGE)
            {
                fprintf(stderr, "range error (%s --> float)\n", c_str());
                errno = 0;
            }
            return res;
        }

        std::vector<long> find_all_long() const noexcept { return find_all_number<long>('l'); }

        std::vector<double> find_all_double() const noexcept { return find_all_number<double>('d'); }

        std::vector<std::size_t> find_all(const char *data) const noexcept
        {
            const auto DATA_LEN = strlen(data);
            const auto LEN = size();
            std::vector<size_t> res;
            size_t last_idx = 0LL;
            while (last_idx <= LEN - DATA_LEN)
            {
                auto pos = find(data, last_idx);
                if (pos == std::string::npos) break;
                res.emplace_back(pos);
                last_idx = pos + DATA_LEN;
            }
            return res;
        }

        std::size_t count(char ch) const noexcept { return std::count(cbegin(), cend(), ch); }

        String toLower_copy() const noexcept
        {
            String res = *this;
            res.toLower();
            return res;
        }
        String &toLower() noexcept
        {
            const size_t SIZE = (*this).size();
            for (size_t i = 0; i < SIZE; ++i)
            {
                char temp = (*this)[i];
                if (isalpha(temp))
                {
                    (*this)[i] = tolower(temp);
                    continue;
                }
            }
            return *this;
        }

        String toUpper_copy() const noexcept
        {
            String res = *this;
            res.toUpper();
            return res;
        }
        String &toUpper() noexcept
        {
            const size_t SIZE = this->size();
            char temp;
            for (size_t i = 0; i < SIZE; ++i)
            {
                temp = (*this)[i];
                if (isalpha(temp)) (*this)[i] = toupper(temp);
            }
            return *this;
        }

        String toCap_copy() const noexcept
        {
            String res{*this};
            return res.toCap();
        }
        String &toCap() noexcept
        {
            bool flag = true;
            const size_t SIZE = (*this).size();
            char temp;
            for (size_t i = 0; i < SIZE; ++i)
            {
                temp = (*this)[i];
                while (isspace(temp))
                {
                    flag = true;
                    temp = (*this)[++i];
                }

                while (!isalpha(temp) && !isspace(temp)) temp = (*this)[++i];

                if (flag && isalpha(temp))
                {
                    (*this)[i] = toupper(temp);
                    flag = false;
                }
            }
            return *this;
        }

        bool startsWith(const std::string &begin) const noexcept
        {
            size_t begin_size = begin.size();
            return std::string::find(begin) == 0;
        }

        bool endsWith(const std::string &end) const noexcept
        {
            const size_t END_SIZE = end.size();
            const size_t SIZE = size();
            if (END_SIZE > SIZE) return false;
            const size_t DIFF = SIZE - END_SIZE;
            const String &this_obj = *this;
            for (size_t i = 0; i < END_SIZE; i++)
            {
                if (this_obj[DIFF + i] != end[i]) return false;
            }
            return true;
        }

        bool startsWith(const String &begin) const noexcept
        {
            return this->startsWith(static_cast<std::string>(begin));
        }
        bool endsWith(const String &end) const noexcept { return this->endsWith(static_cast<std::string>(end)); }

        // bool startsWith(const char *begin) const noexcept;
        // bool endsWith(const char *end) const noexcept;

        bool contains(const char *sub) const noexcept { return strstr(data(), sub); }

        bool contains(const std::string &sub) const noexcept { return contains(sub.data()); }

        String operator*(const size_t N) const noexcept
        {
            String res;
            const size_t SIZE = this->size();
            res.resize(N * SIZE);
            for (size_t i = 0; i < N; i++)
            {
                std::copy(this->cbegin(), this->cend(), res.begin() + i * SIZE);
            }
            return res;
        }
        friend String operator*(const size_t N, const String &ws) { return ws * N; }
        void operator*=(const size_t N) noexcept { *this = (*this) * N; }

        String replace_substr(const char *old, const char *new_) const noexcept
        {
            String copy = *this;
            auto positions = copy.find_all(old);
            const int old_len = strlen(old);
            const int new_len = strlen(new_);
            for (int i = 0; i < positions.size(); i++)
            {
                int position = positions[i];
                // std::cout << "here " << position << std::endl;
                position += i * (new_len - old_len);
                copy.replace(copy.begin() + position, copy.begin() + position + old_len, new_);
            }
            return copy;
        }

        String replace_char(char old, char new_) const noexcept
        {
            return replace_char_fn_copy([old, new_](char ch) { return ch == old ? new_ : ch; });
        }

    private:
        template <typename T>
        std::vector<T> find_all_number(char type) const noexcept
        {
            const auto &fn = [](char ch) {
                if (!isdigit(ch) && ch != '.' && ch != 'e' && ch != '+' && ch != '-') return ' ';
                return ch;
            };

            String spaceSaperated = replace_char_fn_copy(fn);

            std::vector<T> res;
            T i;
            char *end;
            const char *beg = spaceSaperated.c_str();
            while (true)
            {
                switch (type)
                {
                case 'l': i = strtol(beg, &end, 10); break;
                case 'd': i = strtod(beg, &end); break;
                default: printf("unknow type %c\n", type); goto end;
                }
                if (errno == ERANGE)
                {
                    fprintf(stderr, "range error (%s --> long)\n");
                    errno = 0;
                }
                if (beg == end) break;
                res.push_back(i);
                beg = end;
            }
        end:
            return res;
        }

        String replace_char_fn_copy(const std::function<char(char)> &func) const noexcept
        {
            String res;
            res.resize(this->size());
            std::transform(cbegin(), cend(), res.begin(), func);
            return res;
        }
    };
} // namespace wwc