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

#include "profiling_timers.h"

#include "gtest/gtest.h"

#include <iostream>
#include <thread>
#include <chrono>

#include <omp.h>

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


TEST(TimersTest, CorrectUsage)
{
    using namespace profile_timers;

    ASSERT_NO_THROW(
    {
        timer_tic("label1");
            sleep(200.0);
        timer_toc("label1");

        timer_tic("label2");
            sleep(100.0);
        timer_toc("label2");


        timer_tic("label3");

            sleep(100.0);

            timer_tic("label4");

                sleep(50.0);

                timer_tic("label 5");
                    sleep(10.0);
                timer_toc("label 5");

                timer_tic("label 6");
                    sleep(10.0);
                timer_toc("label 6");

            timer_toc("label4");

            timer_tic("label4");

                sleep(50.0);

                timer_tic("label 5");
                    sleep(10.0);
                timer_toc("label 5");

                timer_tic("label 6");
                    sleep(10.0);
                timer_toc("label 6");

            timer_toc("label4");

        timer_toc("label3");

        timer_tic("for loop");
            for (size_t i = 0; i < 100; ++i)
            {
                timer_tic("inner iteration");
                sleep(10.0);
                timer_toc("inner iteration");
            }
        timer_toc("for loop");

        timers_report();
    });

    timers_reset();
}

TEST(TimersTest, CorrectUsageLongLabels)
{
    using namespace profile_timers;

    ASSERT_NO_THROW(
    {
        timer_tic("a very very very very long label1");
            sleep(200.0);
        timer_toc("a very very very very long label1");

        timer_tic("label2");
            sleep(100.0);
        timer_toc("label2");

        timer_tic("label3");
            sleep(100.0);
            timer_tic("label4");
                sleep(50.0);
                timer_tic("label 5");
                    sleep(10.0);
                timer_toc("label 5");
                timer_tic("label 6");
                    sleep(10.0);
                timer_toc("label 6");
            timer_toc("label4");
        timer_toc("label3");
    });

    timers_report();
    timers_reset();

    ASSERT_NO_THROW(
    {
        timer_tic("label1");
            sleep(200.0);
        timer_toc("label1");

        timer_tic("label2");
            sleep(100.0);
        timer_toc("label2");

        timer_tic("label3");
            sleep(100.0);
            timer_tic("label4");
                sleep(50.0);
                timer_tic("a very very very very long label 5");
                    sleep(10.0);
                timer_toc("a very very very very long label 5");
                timer_tic("label 6");
                    sleep(10.0);
                timer_toc("label 6");
            timer_toc("label4");
        timer_toc("label3");
    });

    timers_report();
    timers_reset();
}

TEST(TimersTest, NothingToReport)
{
    using namespace profile_timers;

    ASSERT_NO_THROW(
    {
        timers_reset();
        timers_report();
    });
}

TEST(TimersTest, FailWrongLabel)
{
    using namespace profile_timers;

    ASSERT_THROW(
    {
        timer_tic("label1");
            sleep(200.0);
        timer_toc("label2");
    }, std::runtime_error);

    timers_reset();
}

TEST(TimersTest, FailNotStarted)
{
    using namespace profile_timers;

    ASSERT_THROW(
    {
        timer_toc("label2");
    }, std::runtime_error);

    timers_reset();
}

TEST(TimersTest, FailNotYetStopped)
{
    using namespace profile_timers;

    ASSERT_THROW(
    {
        timer_tic("label1");
            sleep(200.0);
        timers_report();
    }, std::runtime_error);

    timers_reset();
}

TEST(TimersTest, FailNotNested)
{
    using namespace profile_timers;

    ASSERT_THROW(
    {
        // Should this be allowed? Probably not...
        timer_tic("label5");
            timer_tic("label6");
        timer_toc("label5");
            timer_toc("label6");

        timers_report();
    }, std::runtime_error);

    timers_reset();
}

static void thread(const int i)
{
    using namespace profile_timers;

    std::stringstream ss;
    ss << "thread " << i;

    bool verbose = false;
    if (verbose)
        std::cout << "Hello from " << ss.str() << " with thread id " << std::this_thread::get_id() << ".\n";

    timer_tic(ss.str().c_str());
        sleep(250.0);
    timer_toc(ss.str().c_str());
}

TEST(ThreadedTimersTest, CorrectUsageManualThreads)
{
    using namespace profile_timers;

    timer_tic("top level");

    size_t n_threads = 16;
    std::vector<std::thread> threads(n_threads);
    for (size_t i = 0; i < n_threads; ++i)
        threads[i] = std::thread(thread, i);

    for (size_t i = 0; i < n_threads; ++i)
        threads[i].join();

    timer_tic("some more work");
    timer_toc("some more work");

    timer_toc("top level");

    timers_report();

    timers_reset();
}


TEST(ThreadedTimersTest, CorrectUsageOpenMP)
{
    using namespace profile_timers;

    timer_tic("top level");

    int n_iterations = 100;
    bool verbose = false;

    #pragma omp parallel for
    for (int i = 0; i < n_iterations; ++i)
    {
        if (verbose)
        {
            std::stringstream ss;
            ss << "nr omp threads: " << omp_get_num_threads()
               << ", this is thread " << omp_get_thread_num() << "\n";
            std::cout << ss.str();
        }

        timer_tic("inside omp loop");
            sleep(50.0);
        timer_toc("inside omp loop");
    }

    timer_tic("some more work");
    timer_toc("some more work");

    timer_toc("top level");

    timers_report();

    timers_reset();
}
