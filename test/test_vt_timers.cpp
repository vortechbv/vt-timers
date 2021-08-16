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

#include <gtest/gtest.h>

#include <iostream>
#include <thread>
#include <chrono>

#include <omp.h>


TEST(TimersTest, CorrectUsage)
{
    ASSERT_NO_THROW(
    {
        vt_timer_tic("label1");
            sleep(200.0);
        vt_timer_toc("label1");

        vt_timer_tic("label2");
            sleep(100.0);
        vt_timer_toc("label2");


        vt_timer_tic("label3");

            sleep(100.0);

            vt_timer_tic("label4");

                sleep(50.0);

                vt_timer_tic("label 5");
                    sleep(10.0);
                vt_timer_toc("label 5");

                vt_timer_tic("label 6");
                    sleep(10.0);
                vt_timer_toc("label 6");

            vt_timer_toc("label4");

            vt_timer_tic("label4");

                sleep(50.0);

                vt_timer_tic("label 5");
                    sleep(10.0);
                vt_timer_toc("label 5");

                vt_timer_tic("label 6");
                    sleep(10.0);
                vt_timer_toc("label 6");

            vt_timer_toc("label4");

        vt_timer_toc("label3");

        vt_timer_tic("for loop");
            for (size_t i = 0; i < 100; ++i)
            {
                vt_timer_tic("inner iteration");
                sleep(10.0);
                vt_timer_toc("inner iteration");
            }
        vt_timer_toc("for loop");

        vt_timers_to_stdout();
    });

    vt_timers_reset();
}

TEST(TimersTest, CorrectUsageLongLabels)
{
    ASSERT_NO_THROW(
    {
        vt_timer_tic("a very very very very long label1");
            sleep(200.0);
        vt_timer_toc("a very very very very long label1");

        vt_timer_tic("label2");
            sleep(100.0);
        vt_timer_toc("label2");

        vt_timer_tic("label3");
            sleep(100.0);
            vt_timer_tic("label4");
                sleep(50.0);
                vt_timer_tic("label 5");
                    sleep(10.0);
                vt_timer_toc("label 5");
                vt_timer_tic("label 6");
                    sleep(10.0);
                vt_timer_toc("label 6");
            vt_timer_toc("label4");
        vt_timer_toc("label3");
    });

    vt_timers_to_stdout();
    vt_timers_reset();

    ASSERT_NO_THROW(
    {
        vt_timer_tic("label1");
            sleep(200.0);
        vt_timer_toc("label1");

        vt_timer_tic("label2");
            sleep(100.0);
        vt_timer_toc("label2");

        vt_timer_tic("label3");
            sleep(100.0);
            vt_timer_tic("label4");
                sleep(50.0);
                vt_timer_tic("a very very very very long label 5");
                    sleep(10.0);
                vt_timer_toc("a very very very very long label 5");
                vt_timer_tic("label 6");
                    sleep(10.0);
                vt_timer_toc("label 6");
            vt_timer_toc("label4");
        vt_timer_toc("label3");
    });

    vt_timers_to_stdout();
    vt_timers_reset();
}

TEST(TimersTest, NothingToReport)
{
    ASSERT_NO_THROW(
    {
        vt_timers_reset();
        vt_timers_to_stdout();
    });
}

TEST(TimersTest, FailWrongLabel)
{
    vtErrorCode err1, err2;
    ASSERT_NO_THROW(
    {
        err1 = vt_timer_tic("label1");
            sleep(200.0);
        err2 = vt_timer_toc("label2");
    });

    EXPECT_EQ(err1, vtOK);
    EXPECT_EQ(err2, vtERROR);

    const size_t n = 10;
    char message[n];
    vt_last_error_message(message, n);
    EXPECT_STREQ("Timer wit", message);

    const std::string& cpp_message = vt::last_error_message();
    EXPECT_EQ(cpp_message, std::string("Timer with name 'label2' does not exist, so cannot be stopped!"));

    vt_timers_reset();
}

TEST(TimersTest, FailNotStarted)
{
    ASSERT_NO_THROW(
    {
        vtErrorCode err = vt_timer_toc("label2");
        EXPECT_EQ(err, vtERROR);
    });

    vt_timers_reset();
}

TEST(TimersTest, FailNotYetStopped)
{
    ASSERT_NO_THROW(
    {
        vtErrorCode err1 = vt_timer_tic("label1");
            sleep(200.0);
        vtErrorCode err2 = vt_timers_to_stdout();

        EXPECT_EQ(err1, vtOK);
        EXPECT_EQ(err2, vtERROR);
    });


    vt_timers_reset();
}

TEST(TimersTest, FailNotNested)
{
    ASSERT_NO_THROW(
    {
        vtErrorCode err;

        // Should this be allowed? Probably not...
        err = vt_timer_tic("label5");
        EXPECT_EQ(err, vtOK);

            err = vt_timer_tic("label6");
            EXPECT_EQ(err, vtOK);

        err = vt_timer_toc("label5");
        EXPECT_EQ(err, vtERROR);

            err = vt_timer_toc("label6");
            EXPECT_EQ(err, vtOK);

        err = vt_timers_to_stdout();
        EXPECT_EQ(err, vtERROR);
    });

    vt_timers_reset();
}

static void thread(const int i)
{
    std::stringstream ss;
    ss << "thread " << i;

    bool verbose = false;
    if (verbose)
        std::cout << "Hello from " << ss.str() << " with thread id " << std::this_thread::get_id() << ".\n";

    vt_timer_tic(ss.str().c_str());
        sleep(250.0);
    vt_timer_toc(ss.str().c_str());
}

TEST(ThreadedTimersTest, CorrectUsageManualThreads)
{
    vt_timer_tic("top level");

    size_t n_threads = 16;
    std::vector<std::thread> threads(n_threads);
    for (size_t i = 0; i < n_threads; ++i)
        threads[i] = std::thread(thread, i);

    for (size_t i = 0; i < n_threads; ++i)
        threads[i].join();

    vt_timer_tic("some more work");
    vt_timer_toc("some more work");

    vt_timer_toc("top level");

    vt_timers_to_stdout();

    vt_timers_reset();
}


TEST(ThreadedTimersTest, CorrectUsageOpenMP)
{
    vt_timer_tic("top level");

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

        vt_timer_tic("inside omp loop");
            sleep(50.0);
        vt_timer_toc("inside omp loop");
    }

    vt_timer_tic("some more work");
    vt_timer_toc("some more work");

    vt_timer_toc("top level");

    vt_timers_to_stdout();

    vt_timers_reset();
}

#ifdef VT_TIMERS_BUILD_FORTRAN_LIBS 
    extern "C" int f_test_correct_usage();
    extern "C" int f_correct_usage_omp();
    
    TEST(Fortran_API, CorrectUsage)
    {
        ASSERT_EQ(f_test_correct_usage(),vtOK);
    }

    TEST(Fortran_API, CorrectUsageOpenMP)
    {
        ASSERT_EQ(f_correct_usage_omp(),vtOK);
    }
#endif
