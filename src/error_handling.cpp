#include <vt/error_handling.hpp>

#include <cstring>


namespace vt
{

std::string last_error_message_;
vtErrorCode last_error_code_;

VT_C_API void VT_C_CALLCONV vt_last_error_message(char* cstring, const size_t n)
{

    strncpy(cstring, last_error_message_.c_str(), n - 1);
    cstring[n - 1] = '\0';
}

VT_C_API vtErrorCode VT_C_CALLCONV vt_last_error_code()
{
    return last_error_code_;
}


}
