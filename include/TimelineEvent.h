
#ifndef _TIMELINEEVENT_H_
#define _TIMELINEEVENT_H_

namespace GoBattleSim
{

typedef unsigned char Player_Index_t;

enum class EventType : unsigned char
{
    None,
    Announce,
    Free,
    Fast,
    Charged,
    Damage,
    Dodge,
    Effect,
    Enter,
    Exit
};

struct TimelineEvent
{
    int time{0};
    EventType type{EventType::None};
    Player_Index_t player{0};
    short value{0};

    // workaround for C++ 11
    TimelineEvent(int _time = 0, EventType _type = EventType::None, Player_Index_t _player = 0, short _value = 0) : time(_time), type(_type), player(_player), value(_value) {}
};

inline bool operator<(const TimelineEvent &lhs, const TimelineEvent &rhs)
{
    return lhs.time > rhs.time || (lhs.time == rhs.time && lhs.player > rhs.player);
}

} // namespace GoBattleSim

#endif
