#include "CppUTest/CommandLineTestRunner.h"

int main(int ac, char** av)
{
    MemoryLeakWarningPlugin::turnOffNewDeleteOverloads();
    return RUN_ALL_TESTS(ac, av);
}
