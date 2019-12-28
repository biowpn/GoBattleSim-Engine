
#ifndef _TIMELINE_H
#define _TIMELINE_H

namespace GoBattleSim
{

enum EventType
{
	etype_None,
	etype_Announce,
	etype_Free,
	etype_Fast,
	etype_Charged,
	etype_Dodge,
	etype_Enter
};

class TimelineEvent
{
public:
	TimelineEvent(EventType = etype_None, int = 0, int = 0, int = 0);

	inline bool operator<(const TimelineEvent &) const;

	EventType type;
	int time;
	int player;
	int value;
};

class Timeline
{
public:
	Timeline();
	~Timeline();

	void put(const TimelineEvent &);
	TimelineEvent get();
	bool is_empty();
	void erase();

protected:
	void reallocate();

	TimelineEvent *m_array;
	int m_size;
	int m_max_size;
};

} // namespace GoBattleSim

#endif