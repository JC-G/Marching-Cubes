#include "Timing.h"
Timing::Timing() {
    lastTime = 0;
    timeDiff();
}

long Timing::timeDiff() {
    //https://stackoverflow.com/a/19555298
    using namespace std::chrono;
    long ms = duration_cast< milliseconds >(
        system_clock::now().time_since_epoch()
    ).count();
    long result = ms - lastTime;
    lastTime = ms;
    return result;
}