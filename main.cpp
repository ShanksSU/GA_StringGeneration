#include <iostream>
#include "lib/GA.h"

int main() {
    std::string targetStr = "Hello World!";
    printf_s("Result string：%s\n", entry(targetStr).c_str());
    return 0;
}
