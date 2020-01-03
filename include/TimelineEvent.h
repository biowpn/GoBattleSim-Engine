
#ifndef _TIMELINEEVENT_H_
#define _TIMELINEEVENT_H_

namespace GoBattleSim
{
enum class EventType
{
    None,
    Announce,
    Free,
    Fast,
    Charged,
    Dodge,
    Enter
};

struct TimelineEvent
{
    EventType type;
    int time;
    unsigned player;
    int value;
};

inline bool operator<(const TimelineEvent &lhs, const TimelineEvent &rhs)
{
    return lhs.time > rhs.time || (lhs.time == rhs.time && lhs.player > rhs.player);
}

} // namespace GoBattleSim

#endif
