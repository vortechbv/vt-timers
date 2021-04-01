#include <vt/timers.hpp>
#include "sleep.h"
#include <chrono>
void sleep(const double milliseconds)
{
    using namespace std::chrono;
    auto t0 = high_resolution_clock::now();
    duration<double> duration(0.0);
    while (duration.count() < milliseconds / 1000.0)
        duration = high_resolution_clock::now() - t0;

//     //ALTERNATIVE:
//     std::this_thread::sleep_for(std::chrono::duration<double, std::milli>(milliseconds));
}

