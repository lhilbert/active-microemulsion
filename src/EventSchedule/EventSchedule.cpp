//
// Created by tommaso on 13/09/18.
//

#include "EventSchedule.h"

template <typename EventType>
void EventSchedule<EventType>::addEvent(double time, EventType eventType, double timeMultiplier)
{
    schedule[time*timeMultiplier].push_back(eventType);
    nextEventTime = schedule.begin()->first;
}

template<typename EventType>
void EventSchedule<EventType>::addEvents(double startTime, double endTime, double timeStep, EventType eventType,
                                         double timeMultiplier)
{
    for (double t = startTime; t <= endTime; t += timeStep)
    {
        addEvent(t, eventType, timeMultiplier);
    }
}

template <typename EventType>
void EventSchedule<EventType>::addEvents(std::vector<double> times, EventType eventType, double timeMultiplier)
{
    if (times.empty())
    {
        addEvent(defaultEventTime, eventType, timeMultiplier);
    }
    else
    {
        for (double t : times)
        {
            addEvent(t, eventType, timeMultiplier);
        }
    }
}

template <typename EventType>
std::vector<EventType> EventSchedule<EventType>::popEventsToApply(double t)
{
    std::vector<EventType> result;
    auto endIt = schedule.upper_bound(t);
    for (auto it = schedule.begin(); it != endIt; ++it)
    {
        auto curTimeEventsVector = it->second;
        result.insert(result.end(), curTimeEventsVector.begin(), curTimeEventsVector.end());
    }
    schedule.erase(schedule.begin(), endIt);
    if (schedule.empty())
    {
        nextEventTime = std::numeric_limits<double>::max();
    }
    else
    {
        nextEventTime = schedule.begin()->first;
    }
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

template<typename EventType>
std::vector<double> EventSchedule<EventType>::getAllEventsTimes()
{
    std::vector<double> times;
    for (auto it = schedule.begin(); it != schedule.end(); ++it)
    {
        times.push_back(it->first);
    }
    return times;
}

template<typename EventType>
double EventSchedule<EventType>::getLastEventTime()
{
    return schedule.rbegin()->first;
}

template<typename EventType>
double EventSchedule<EventType>::getNextEventTime() const
{
    return nextEventTime;
}
