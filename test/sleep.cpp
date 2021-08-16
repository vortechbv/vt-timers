// Copyright (c) 2019-2020 VORtech b.v.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
#include <vt/timers.hpp>
#include "sleep.h"
#include <chrono>
#include <thread>

VT_C_API void VT_C_CALLCONV sleep(const double milliseconds) 
{
#   ifdef BUSY
        using namespace std::chrono;
        auto t0 = high_resolution_clock::now();
        duration<double> duration(0.0);
        while (duration.count() < milliseconds / 1000.0)
            duration = high_resolution_clock::now() - t0;
#   else
        //ALTERNATIVE:
        std::this_thread::sleep_for(std::chrono::duration<double, std::milli>(milliseconds));
#   endif
}

