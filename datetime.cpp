#include "datetime.h"

namespace base {

daytime daytime::now() {
    auto now_nano = system_clock::now().time_since_epoch();
    auto day = duration_cast<days>(now_nano);
    auto nano = now_nano - day;
    return daytime(nano);
}

daytime daytime::create(rep hr, rep min, rep sec, rep nanosec) {
    return hours(hr) + minutes(min) + seconds(sec) + nanoseconds(nanosec) - time_offset();
}

daytime daytime::from_string(std::string str) {
    rep secs{0};
    auto len_str = str.length();
    if (len_str != 8 && len_str != 12 && len_str != 15 && len_str != 18) {
        throw format_error("Invalid daytime format (format supported is HH:MM:SS[.fff[fff[fff]]])");
    }
    int pos = 0;
    for (int i = 0; i < 3; ++i) {
        auto t = stol(str.substr(pos, 2));
        pos += 2;

        auto next_char = str.substr(pos, 1);

        secs *= 60;
        secs += t;
        if (i >= 2) {
            break;
        }

        if (next_char != ":") {
            throw("Invalid time separator");
        }

        pos += 1;
    }

    if (pos < len_str) {
        if (str.substr(pos, 1) != ".") {
            throw("Invalid nano component");
        } else {
            pos += 1;
            auto len_remainder = len_str - pos;
            auto nano = stol(str.substr(pos, len_remainder));
            if (len_remainder == 3) {
                nano *= 1000000;
            } else if (len_remainder == 6) {
                nano *= 1000;
            }
            return seconds(secs) + nanoseconds(nano) - time_offset();
        }
    }
    return seconds(secs) - time_offset();
}

nanoseconds daytime::time_offset() {
    time_t t{0};
    auto ptm = localtime(&t);
    return (hours(ptm->tm_hour) + minutes(ptm->tm_min) + seconds(ptm->tm_sec));
}

date date::today() { return time_point_cast<days>(system_clock::now()).time_since_epoch(); }

date date::create(rep year, rep mon, rep day) { return years(year - 1970) + months(mon - 1) + days(day - 1); }
date date::from_string(std::string str) {
    auto len_str = str.length();
    if (len_str != 8) {
        throw format_error("Invalid date format (format supported is YYYYMMDD)");
    }

    auto y = stol(str.substr(0, 4));
    auto m = stol(str.substr(4, 2));
    auto d = stol(str.substr(6, 2));

    return years(y - 1970) + months(m - 1) + days(d - 1);
}

datetime::datetime(base::date date, base::daytime daytime)
    : datetime_base(duration_cast<nanoseconds>(date + daytime)) {}

datetime datetime::create(rep year, rep mon, rep day, rep hour, rep min, rep sec, rep nanosec) {
    return datetime(date::create(year, mon, day), daytime::create(hour, min, sec, nanosec));
}

datetime datetime::now() { return system_clock::now().time_since_epoch(); }
datetime datetime::from_string(std::string str) {
    auto len_str = str.length();
    if (len_str != 17 && len_str != 21 && len_str != 24 && len_str != 27) {
        throw format_error("Invalid datetime format (format supported is YYYYMMDD HH:MM:SS[.fff[fff[fff]]])");
    }

    auto date_str = str.substr(0, 8);
    auto date = date::from_string(date_str);
    auto time_str = str.substr(9, len_str - 9);
    auto time = daytime::from_string(time_str);

    return datetime(date, time);
}

base::date datetime::date() const { return duration_cast<days>(time_since_epoch()); }
base::daytime datetime::daytime() const { return time_since_epoch() - date(); }

} // namespace base