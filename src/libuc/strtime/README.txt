TIMESTR

These are (very) old subroutines that format UNIX® time into c-strings.

Example C++ code:

--------------------------------------------------------------------------------

#include <chrono>
#include <iostream>

using namespace std::chrono;

void print_broken_down_time(const time_zone* zone,system_clock::time_point tp) {
    // 1. Combine zone and time point
    zoned_time zt{zone, tp};

    // 2. Get the local time representation
    auto lt = zt.get_local_time();
    auto day_point = floor<days>(lt); // The date portion

    // 3. Break out the components
    year_month_day ymd{day_point}; // Equivalent to tm_year, tm_mon, tm_mday
    hh_mm_ss hms{lt - day_point}; // Equivalent to tm_hour, tm_min, tm_sec

    // Accessing values (all are type-safe objects)
    std::cout << "Year: "  << ymd.year()
              << " Month: " << (unsigned)ymd.month()
              << " Day: "   << (unsigned)ymd.day() << "\n";

    std::cout << "Time: "  << hms.hours().count() << ":"
              << hms.minutes().count() << ":"
              << hms.seconds().count() << "\n";
} /* end subroutine */

--------------------------------------------------------------------------------


