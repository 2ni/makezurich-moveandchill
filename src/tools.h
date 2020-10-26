#ifndef __TOOLS__
#define __TOOLS__

#include "Arduino.h"

#define NOK(str)  "\033[31;1m" str "\033[0m"  // output in red
#define OK(str)   "\033[32;1m" str "\033[0m"  // output in green
#define WARN(str) "\033[33;1m" str "\033[0m"  // output in yellow
#define BOLD(str) "\033[1m" str "\033[0m"     // output bold

void blink(byte amount);
void hello();
void scan_i2c();
void low_power();

#endif
