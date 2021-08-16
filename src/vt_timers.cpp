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

#include <vt/timers.hpp>
#include <vt/timers.h>
#include <vt/error_handling.hpp>

#include <chrono>
#include <iostream>
#include <string>
#include <iomanip>
#include <algorithm>
#include <utility>
#include <vector>
#include <sstream>
#include <thread>
#include <mutex>
#include <cstring>

#include <omp.h>

namespace vt {

// Note: statics are destructed after thread_locals, according to the standard.

// There is a global set of Timers, initially empty.
static std::map<std::thread::id, Timer> timers;

static const double ms_per_second  = 1000.,
                    s_per_second   = 1.,
                    min_per_second = 1./60.,
                    hr_per_second = 1./60./60.;

// Each thread first keeps its own set of Timers.
static thread_local Timer toplevel;
static thread_local Timer* current_level = nullptr;

// If a thread is finishing, its Timers are moved to the global set.
static void collect_timer_from_this_thread()
{
    static std::mutex timers_mutex;
    std::lock_guard<std::mutex> lock(timers_mutex);
    toplevel.stop();
    if (toplevel.children_count() != 0)
        timers.emplace(std::this_thread::get_id(), std::move(toplevel));
    toplevel.reset();
}

struct AtThreadExit
{
    ~AtThreadExit()
    {
        collect_timer_from_this_thread();
    }
};

static thread_local AtThreadExit at_thread_exit;




Timer::Timer()
{
    this->reset();
}


void Timer::reset()
{
    using namespace std::chrono;

    is_running_ = false;
    children_.clear();
    parent_ = nullptr;
    wall_time_ = duration<double>(0.0);
    cpu_time_ = duration<double>(0.0);
    nr_calls_ = 0;
}


void Timer::start()
{
    using namespace std::chrono;

    is_running_ = true;
    start_ = high_resolution_clock::now();

     nr_calls_ += 1;
}


void Timer::stop()
{
    using namespace std::chrono;

    auto end = high_resolution_clock::now();
    is_running_ = false;

    wall_time_ += end - start_;
}


bool Timer::is_running() const
{
    return is_running_;
}


bool Timer::has_timer_with_name(const std::string& name) const
{
    try {
        children_.at(name);  // throws if name is not a valid key
    }
    catch (const std::out_of_range&) {
        return false;
    }
    return true;
}


size_t Timer::children_count() const
{
    return children_.size();
}


Timer& Timer::new_or_existing_child(const std::string& name)
{
    // operator[] creates a new Timer if it does not exist under this name
    return children_[name];
}


size_t Timer::max_label_length_recursive() const
{
    size_t max_label_length = 0;
    for (const auto& child : children_)
    {
        const Timer& timer = child.second;
        const std::string& name = child.first;

        max_label_length = std::max(max_label_length, name.length());
        max_label_length = std::max(max_label_length, timer.max_label_length_recursive());
    }
    return max_label_length;
}


std::string Timer::tree_string(const std::string& name, const size_t level, const size_t label_length, const double conversion) const
{
    std::stringstream out;

    if ( nr_calls_ == 0 && children_.size() == 0)
        return out.str();

    // print own timings
    std::string indent(level, ' ');
    std::stringstream nr_calls_ss;
    nr_calls_ss << "(" << nr_calls_ << ")";
    out << indent << std::setw(label_length) << std::left << name
        << "  " << std::setw(8) << wall_time_.count() * conversion
        << std::setw(7) << nr_calls_ss.str() << "\n";

    // copy children into vector, since vector has a random access iterator that will be used by std::sort
    typedef std::pair<std::string, Timer> TimerLabelPair;
    std::vector<TimerLabelPair> children(children_.begin(), children_.end());

    // sort the children
    auto longest_first = [](const TimerLabelPair& a,
                            const TimerLabelPair& b)
    {
        return a.second.wall_time_ > b.second.wall_time_;
    };
    std::sort(children.begin(), children.end(), longest_first);

    // print children's timings
    for (const auto& child : children) {
        const Timer& timer = child.second;
        const std::string& name = child.first;
        out << timer.tree_string(name, level + 3, label_length,conversion);
    }

    // print remaining time
    if (children.size() > 0) {
        std::chrono::duration<double> children_time(0.0);
        for (const auto& child : children) {
            const Timer& timer = child.second;
            children_time += timer.wall_time_;
        }
        std::chrono::duration<double> other_time = wall_time_ - children_time;

        // only report if other > 1% of total time
        std::chrono::duration<double> threshold( wall_time_ * 1.0 / 100.0);
        if (other_time > threshold) {
            indent = std::string(level + 3, ' ');
            out << indent << std::setw(label_length) << std::left << "(other)"
                << "  " << std::setw(8) << other_time.count() * conversion << "\n";
        }
    }

    return out.str();
}


static void timers_collect()
{
    // Retrieve timers from main thread, also for sequential code
    collect_timer_from_this_thread();

    // Retrieve timers from OpenMP threads
    #pragma omp parallel for
    for (int i = 0; i < omp_get_max_threads(); ++i)
    {
        // start from thread 1, since this thread is omp thread 0
        int tid = omp_get_thread_num();
        if (tid == 0) continue;

        collect_timer_from_this_thread();
    }
    #pragma omp barrier  // necessary?
}

VT_TIMERS_ATTR double largest_time() 
{
    double retval = 0.0;
    // All timers should be in static timers map
    for (auto id_and_timer = timers.begin();
         id_and_timer != timers.end();
         ++id_and_timer)
    {
        retval = std::max(retval,id_and_timer->second.wall_time());
    }
    return retval;
}

VT_TIMERS_ATTR void timers_to_stream(std::ostream& out)
{
    if (current_level != &toplevel && current_level != nullptr)
        throw std::runtime_error("Not all timers have been stopped!");

    if (current_level == nullptr && timers.size() == 0)
    {
        out << "No timings to report.\n";
        return;
    }

    timers_collect();
    const double large = largest_time();
    const double conversion = 
            large*ms_per_second  < 1000.0 ? ms_per_second 
         :  large*s_per_second   < 1000.0 ? s_per_second  
         :  large*min_per_second < 1000.0 ? min_per_second  
         :  hr_per_second;
    const char *unit = 
            large*ms_per_second  < 1000.0 ? "ms"
         :  large*s_per_second   < 1000.0 ? "s"
         :  large*min_per_second < 1000.0 ? "min"
         :  "hr";
         
    out << "Collected timer info from " << timers.size()
        << " thread" << (timers.size() == 1 ? "" : "s") << "\n";

    out << "Timing report, all times in ["<<unit<<"] : \n";

    std::thread::id main_thread_id = std::this_thread::get_id();
    // All timers should be in static timers map
    for (auto id_and_timer = timers.begin();
         id_and_timer != timers.end();
         ++id_and_timer)
    {
        std::thread::id tid = id_and_timer->first;
        std::string thread_id;
        if (tid == main_thread_id)
        {
            thread_id = "Main thread";
        }
        else
        {
            std::stringstream ss;
            ss << "thread id " << tid;
            thread_id = ss.str();
        }

        const Timer& timer = id_and_timer->second;
        size_t min_label_length = 10;
        size_t max_label_length = std::max(thread_id.size(), timer.max_label_length_recursive());
        size_t label_length = std::max(min_label_length, max_label_length);

        out << timer.tree_string(thread_id, 0, label_length, conversion);
    }
}


VT_TIMERS_ATTR std::string timers_to_string()
{
    std::stringstream out;
    timers_to_stream(out);
    return out.str();
}


}  // namespace vt



VT_C_API vtErrorCode VT_C_CALLCONV vt_timer_tic(const char* name) VT_EXCEPT_TO_ERRORCODE(
{
    using namespace vt;

    if (current_level == nullptr) {
        current_level = &toplevel;
        toplevel.start();
    }

    Timer& timer = current_level->new_or_existing_child(name);
    if (timer.is_running())
        throw std::runtime_error("Timer is already running!");

    timer.parent_ = current_level;
    current_level = &timer;

    timer.start();

    return vtOK;
})


VT_C_API vtErrorCode VT_C_CALLCONV vt_timer_toc(const char* name) VT_EXCEPT_TO_ERRORCODE(
{
    using namespace vt;

    if (current_level == nullptr) {
        throw std::runtime_error("No started timers available!");
    }

    bool check_name = true;
    if (check_name) {
        if (current_level->parent_ == nullptr ||
                ! current_level->parent_->has_timer_with_name(name)) {
            std::stringstream ss;
            ss << "Timer with name '" << name << "' does not exist, so cannot be stopped!";
            throw std::runtime_error(ss.str());
        }
    }

    Timer* timer = current_level;
    timer->stop();

    current_level = current_level->parent_;

    return vtOK;
})


VT_C_API vtErrorCode VT_C_CALLCONV vt_timers_to_cstring(char* cstring, const size_t n) VT_EXCEPT_TO_ERRORCODE(
{
    using namespace vt;

    std::stringstream out;
    timers_to_stream(out);
    strncpy(cstring, out.str().c_str(), n - 1);
    cstring[n - 1] = '\0';

    return vtOK;
})


VT_C_API vtErrorCode VT_C_CALLCONV vt_timers_to_stdout() VT_EXCEPT_TO_ERRORCODE(
{
    using namespace vt;

    timers_to_stream(std::cout);

    return vtOK;
})


VT_C_API vtErrorCode VT_C_CALLCONV vt_timers_reset() VT_EXCEPT_TO_ERRORCODE(
{
    using namespace vt;

    timers_collect();
    timers.clear();
    current_level = nullptr;

    return vtOK;
})
