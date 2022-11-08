
#include "NativeTestEngine.h"

extern void addFillRangesTest();
extern void addRangesNextTest();

int main()
{
    addFillRangesTest();
    addRangesNextTest();

    NativeTests::self().run();
    NativeTests::self().deinit();
}
