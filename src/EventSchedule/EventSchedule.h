//
// Created by tommaso on 13/09/18.
//

#ifndef ACTIVE_MICROEMULSION_CUTOFFEVENTSCHEDULE_H
#define ACTIVE_MICROEMULSION_CUTOFFEVENTSCHEDULE_H

#include <map>
#include <vector>
#include <limits>

typedef enum CutoffEvent
{
    ACTIVATE,
    FLAVOPIRIDOL,
    ACTINOMYCIN_D,
    TXN_SPIKE,
    CUSTOM_EVENT
} CutoffEvent;

typedef enum SnapshotEvent
{
    NORMAL_SNAPSHOT,
    GENERIC_EXTRA_SNAPSHOT,
    ACTIVATE_SNAPSHOT,
    FLAVOPIRIDOL_SNAPSHOT,
    ACTINOMYCIN_D_SNAPSHOT,
    CUSTOM_EVENT_SNAPSHOT
} SnapshotEvent;

template <typename EventType>
class EventSchedule
{
private:
    double defaultEventTime;
    std::map<double, std::vector<EventType>> schedule;
    double nextEventTime;

public:
    explicit EventSchedule(double defaultEventTime) : defaultEventTime(defaultEventTime),
                                                    nextEventTime(std::numeric_limits<double>::max())
    {}
    
    void addEvent(double time, EventType eventType, double timeMultiplier=1);
    
    void addEvents(double startTime, double endTime, double timeStep, EventType eventType,
                       double timeMultiplier = 1);
    
    void addEvents(std::vector<double> times, EventType eventType, double timeMultiplier=1);
    
    double getNextEventTime() const;
    
    bool check(double t);
    
    std::vector<EventType> popEventsToApply(double t);
    
    std::vector<double> getAllEventsTimes();
    
    double getLastEventTime();
    
    unsigned long size();
};


#endif //ACTIVE_MICROEMULSION_CUTOFFEVENTSCHEDULE_H
