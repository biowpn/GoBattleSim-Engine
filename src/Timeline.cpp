
#include "Timeline.h"


TimelineEvent::TimelineEvent(EventType t_type, int t_time, int t_player, int t_value)
: type(t_type), time(t_time), player(t_player), value(t_value)
{
	
}


bool TimelineEvent::operator<(const TimelineEvent& rhs) const
{
	return time < rhs.time || (time == rhs.time && player < rhs.player);
}


std::ostream& operator<<(std::ostream& os, const TimelineEvent& event)
{
	os << event.type << ' ' << event.time << ' ' << event.player << ' ' << event.value;
	return os;
}



Timeline::Timeline()
{
	m_size = 0;
	m_max_size = 16;
	m_array = nullptr;
	reallocate();
}


Timeline::~Timeline()
{
	delete[] m_array;
	m_array = nullptr;
	m_size = 0;
	m_max_size = 0;
}


void Timeline::put(const TimelineEvent& event)
{
	if (m_size >= m_max_size)
	{
		m_max_size *= 2;
		reallocate();
	}
	// Find a proper place (i_child) for {event}
	int i_child = m_size, i_parent = (i_child - 1) / 2;
	while (i_child > 0 && event < m_array[i_parent])
	{
		m_array[i_child] = m_array[i_parent];
		i_child = i_parent;
		i_parent = (i_child - 1) / 2;
	}
	m_array[i_child] = event;
	++m_size;
}


TimelineEvent Timeline::get()
{
	TimelineEvent first = m_array[0], last = m_array[m_size - 1];
	--m_size;
	int i_parent = 0, i_child = 2 * i_parent + 1;
	// Find a proper place (i_parent) for {last}
	while (i_child < m_size)
	{
		if (i_child + 1 < m_size && m_array[i_child + 1] < m_array[i_child])
		{
			i_child += 1;
		}
		if (m_array[i_child] < last)
		{
			m_array[i_parent] = m_array[i_child];
		}
		else
		{
			break;
		}
		i_parent = i_child;
		i_child = 2 * i_parent + 1;
	}
	m_array[i_parent] = last;
	return first;
}


bool Timeline::is_empty()
{
	return m_size == 0;
}


void Timeline::erase()
{
	m_size = 0;
}


void Timeline::reallocate()
{
	TimelineEvent* array_new = new TimelineEvent[m_max_size];
	if (m_array)
	{
		for (int i = 0; i < m_size; ++i)
		{
			array_new[i] = m_array[i];
		}
		delete[] m_array;
	}
	m_array = array_new;
}






