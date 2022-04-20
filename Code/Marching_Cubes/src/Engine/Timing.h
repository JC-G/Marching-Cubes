#ifndef TIMING_H
#define TIMING_H

#include <chrono>

class Timing {
    public:
        Timing();
        long timeDiff();
    private:
        long lastTime;

};


#endif
