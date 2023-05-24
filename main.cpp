#include <iostream>
#include "lib/GA.h"
using namespace std;

int main() {
    string targetStr = "Hello World!";
    printf_s("Result string¡G%s\n", entry(targetStr).c_str());
    return 0;
}
