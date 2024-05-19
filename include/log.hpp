#pragma once
#include <stdarg.h>
//#include <string>
#define LOG_MAXLEN 1024



void outf(const char* fmt, ...); //out thru js console.log()


#define LOGF(fmt, ...) outf(fmt, __VA_ARGS__) 
#define ERRORF(fmt, ...) outf("Error! %s [%d] >>>\n    ", __FILE__, __LINE__); outf(fmt, __VA_ARGS__)  




extern "C" { void console_log (const char* str) ; }