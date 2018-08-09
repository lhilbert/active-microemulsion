//
// Created by tommaso on 03/08/18.
//

#include <bits/types/struct_timeval.h>
#include <cstdlib>
#include <sys/time.h>
#include "Timing.h"

long Timing::getCurrentTimeMillis()
{
    struct timeval timecheck;
    gettimeofday(&timecheck, nullptr);
    long currentTimeMillis = (long)timecheck.tv_sec * 1000 + (long)timecheck.tv_usec / 1000;
    return currentTimeMillis;
}
