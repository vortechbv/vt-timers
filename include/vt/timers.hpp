// Copyright (c) 2019 VORtech b.v.
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

#ifndef VT_TIMERS_HPP
#define VT_TIMERS_HPP

#include <vt/timers.h>

#include <chrono>
#include <map>


namespace vt {

class Timer
{
public:
    Timer();
    void reset();
    void start();
    void stop();

    bool is_running() const;
    bool has_timer_with_name(const std::string& name) const;
    size_t children_count() const;
    Timer& new_or_existing_child(const std::string& name);

    size_t max_label_length_recursive() const;
    std::string tree_string(const std::string& name, const size_t level, const size_t label_length) const;

    Timer* parent_;

private:
    bool is_running_;
    std::map<std::string, Timer> children_;

    std::chrono::time_point<std::chrono::high_resolution_clock> start_;
    std::chrono::duration<double> wall_time_;
    std::chrono::duration<double> cpu_time_;
    unsigned nr_calls_;
};


VT_TIMERS_ATTR void timers_to_stream(std::ostream& stream);

VT_TIMERS_ATTR std::string timers_to_string();


}  // namespace vt

#endif  // VT_TIMERS_HPP
