#pragma once
#include <common.hpp>

extern "C" {
    char *_get_user_agent();
    int is_mobile();
}

inline std::string get_user_agent(){
    char* ua = _get_user_agent();
    auto ret = std::string(ua);
    free(ua);

    return ret;
}
