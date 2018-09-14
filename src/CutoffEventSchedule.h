//
// Created by tommaso on 13/09/18.
//

#ifndef ACTIVE_MICROEMULSION_CUTOFFEVENTSCHEDULE_H
#define ACTIVE_MICROEMULSION_CUTOFFEVENTSCHEDULE_H

#include <map>
#include <vector>

typedef enum CutoffEventType
{
    ACTIVATE,
    FLAVOPIRIDOL,
    ACTINOMYCIN_D,
    CUSTOM_EVENT
} CutoffEventType;

class CutoffEventSchedule
{
private:
    double defaultCutoffTime;
    std::map<double, std::vector<CutoffEventType>> schedule;
    double nextEventTime;

public:
    CutoffEventSchedule(double defaultCutoffTime) : defaultCutoffTime(defaultCutoffTime),
                                                    nextEventTime(0)
    {}
    
    void addEvent(double time, CutoffEventType eventType);
    
    void addEvents(std::vector<double> times, CutoffEventType eventType);
    
    bool check(double t);
    
    std::vector<CutoffEventType> getEventsToApply(double t);
    
    unsigned int size();
};


#endif //ACTIVE_MICROEMULSION_CUTOFFEVENTSCHEDULE_H
