//
// Created by tommaso on 03/08/18.
//

#ifndef ACTIVE_MICROEMULSION_TIMING_H
#define ACTIVE_MICROEMULSION_TIMING_H


class Timing
{
public:
    inline static double getTimeSpentSeconds(long startTimeMillis, long endTimeMillis)
    {
        return (endTimeMillis - startTimeMillis) / 1000.0;
    }
    
    static long getCurrentTimeMillis();
private:
    Timing() = default;
};


#endif //ACTIVE_MICROEMULSION_TIMING_H
