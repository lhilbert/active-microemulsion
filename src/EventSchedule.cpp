//
// Created by tommaso on 13/09/18.
//

#include "EventSchedule.h"

template <typename EventType>
void EventSchedule<EventType>::addEvent(double time, EventType eventType)
{
    schedule[time].push_back(eventType);
}

template <typename EventType>
void EventSchedule<EventType>::addEvents(std::vector<double> times, EventType eventType)
{
    if (times.empty())
    {
        addEvent(defaultEventTime, eventType);
    }
    else
    {
        for (double t : times)
        {
            addEvent(t, eventType);
        }
    }
}

template <typename EventType>
std::vector<EventType> EventSchedule<EventType>::getEventsToApply(double t)
{
    std::vector<EventType> result;
    auto endIt = schedule.upper_bound(t);
    for (auto it = schedule.begin(); it != endIt; ++it)
    {
        auto curTimeEventsVector = it->second;
        result.insert(result.end(), curTimeEventsVector.begin(), curTimeEventsVector.end());
    }
    schedule.erase(schedule.begin(), endIt);
    nextEventTime = schedule.begin()->first;
    return result;
}

template <typename EventType>
unsigned long EventSchedule<EventType>::size()
{
    return schedule.size();
}

template <typename EventType>
bool EventSchedule<EventType>::check(double t)
{
    return t >= nextEventTime;
}

