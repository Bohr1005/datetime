#pragma once
#include <chrono>
#include <stdexcept>
#include <string>

namespace base {

using namespace std::chrono;

class format_error : std::runtime_error {
public:
    format_error(const std::string &msg) : std::runtime_error(msg) {}
};

class daytime : public nanoseconds {
public:
    daytime() = default;
    explicit daytime(rep val) : nanoseconds(val) {}

    template <typename Rep, typename Period>
    daytime(const std::chrono::duration<Rep, Period> &du) : nanoseconds(duration_cast<nanoseconds>(du)) {}

    static daytime now();
    static daytime create(rep hr, rep min = 0, rep sec = 0, rep nanosec = 0);

    // HH:SS:MM[.fff[fff[fff]]]
    static daytime from_string(std::string timestr);

    static nanoseconds time_offset();
};

using days = duration<int32_t, std::ratio<86400, 1>>;
using years = duration<int32_t, std::ratio_multiply<std::ratio<146097, 400>, days::period>>;
using months = duration<int32_t, std::ratio_divide<years::period, std::ratio<12>>>;

class date : public days {
public:
    date() = default;
    explicit date(rep val) : days(val){};

    template <typename Rep, typename Period>
    date(const std::chrono::duration<Rep, Period> &du) : days(duration_cast<days>(du)) {}

    static date today();
    static date create(rep year, rep mon, rep day);
    // YYYYMMDD
    static date from_string(std::string str);
};

using datetime_base = time_point<std::chrono::system_clock, nanoseconds>;

class datetime : public datetime_base {
public:
    datetime() = default;
    explicit datetime(rep val) : datetime_base(std::chrono::nanoseconds(val)){};
    datetime(const duration &du) : datetime_base(du){};
    datetime(base::date date, base::daytime daytime);

    static datetime create(rep year, rep mon, rep day, rep hour = 0, rep min = 0, rep sec = 0, rep nanosec = 0);
    static datetime now();
    static datetime from_string(std::string str);

    base::date date() const;
    base::daytime daytime() const;

    template <typename Rep, typename Period>
    datetime operator+(const std::chrono::duration<Rep, Period> &du) {
        auto nano = std::chrono::duration_cast<duration>(du);
        return datetime{time_since_epoch() + nano};
    }

    template <typename Rep, typename Period>
    datetime operator-(const std::chrono::duration<Rep, Period> &du) {
        auto nano = std::chrono::duration_cast<duration>(du);
        return datetime{time_since_epoch() - nano};
    }
};
} // namespace base