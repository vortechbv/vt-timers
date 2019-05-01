#ifndef ERROR_HANDLING_HPP
#define ERROR_HANDLING_HPP

#include <vt/timers.h>

#include <iostream>

namespace vt {

extern std::string last_error_message_;
extern vtErrorCode last_error_code_;

inline const std::string& last_error_message()
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

