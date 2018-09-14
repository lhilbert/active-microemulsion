//
// Created by tommaso on 13/09/18.
//

#include "CutoffEventSchedule.h"

void CutoffEventSchedule::addEvent(double time, CutoffEventType eventType)
{
    schedule[time].push_back(eventType);
}

void CutoffEventSchedule::addEvents(std::vector<double> times, CutoffEventType eventType)
{
    if (times.empty())
    {
        addEvent(defaultCutoffTime, eventType);
    }
    else
    {
        for (double t : times)
        {
            addEvent(t, eventType);
        }
    }
}

std::vector<CutoffEventType> CutoffEventSchedule::getEventsToApply(double t)
{
    std::vector<CutoffEventType> result;
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

unsigned int CutoffEventSchedule::size()
{
    return schedule.size();
}

bool CutoffEventSchedule::check(double t)
{
    return t >= nextEventTime;
}
