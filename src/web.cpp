#include <web.hpp>
//https://emscripten.org/docs/api_reference/emscripten.h.html#calling-javascript-from-c-c

EM_JS(char*, _get_user_agent,(),{

    //c str = utf 8 js str = utf 16 !!!
    var useragent = window.navigator.userAgent;
    console.log("get_user_agent()");
    return stringToNewUTF8(useragent);
    //window.navigator.userAgent
});


EM_JS(int, is_mobile, (), {
     return /Android|webOS|iPhone|iPad|iPod|BlackBerry|IEMobile|Opera Mini/i.test(navigator.userAgent); //good enough
});