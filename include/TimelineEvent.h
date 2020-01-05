
#ifndef _TIMELINEEVENT_H_
#define _TIMELINEEVENT_H_

namespace GoBattleSim
{

enum class EventType : unsigned char
{
    None,
    Announce,
    Free,
    Fast,
    Charged,
    Damage,
    Dodge,
    Enter
};

struct TimelineEvent
{
    int time;
    EventType type;
    unsigned char player;
    short value;
};

inline bool operator<(const TimelineEvent &lhs, const TimelineEvent &rhs)
{
    return lhs.time > rhs.time || (lhs.time == rhs.time && lhs.player > rhs.player);
}

} // namespace GoBattleSim

#endif
