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

#ifndef ERROR_HANDLING_HPP
#define ERROR_HANDLING_HPP

#include <vt/timers.h>

#include <iostream>

namespace vt {

extern std::string last_error_message_;
extern vtErrorCode last_error_code_;

VT_TIMERS_ATTR inline const std::string& last_error_message()
{
    return last_error_message_;
}
// Note: See timers.h for similar C API functions.


/**
 * A function that catches any exceptions and converts them into an
 * error code. This can be used to ensure that no exceptions pass through the
 * C-API. Can be used as:
 *
 *         return except_to_errcode([&]() -> vtErrorCode
 *         { <function body implementation> });
 *
 * body: a function body that returns an vtErrorCode, in the form of a C++ lambda expression.
 */
template<typename Lambda>
vtErrorCode except_to_errcode(Lambda&& body)
{
    try
    {
        return body();
    }
    catch (const std::exception& ex)
    {
        last_error_message_ = ex.what();
        last_error_code_ = vtERROR;
        return last_error_code_;
    }
    catch (...)
    {
        last_error_message_ = "Unknown exception was triggered.";
        last_error_code_ = vtERROR;
        return last_error_code_;
    }
}

}


/**
 * Can be used to replace the function body by a call to except_to_errcode(), in order to
 * ensure that no exceptions can escape the function. Example usage:
 *
 *     VT_C_API vtErrorCode VT_C_CALLCONV function(<arguments>) VT_EXCEPT_TO_ERRORCODE(
 *     {
 *         <statements>
 *
 *         return vtOK;
 *     })
 */
#define VT_EXCEPT_TO_ERRORCODE(body) \
  { return vt::except_to_errcode([&]() body ); }


#endif  // ERROR_HANDLING_HPP

