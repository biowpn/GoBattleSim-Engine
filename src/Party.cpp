
#include "Party.h"

#include "GameMaster.h"

#include <string.h>
#include <stdexcept>
#include <stdio.h>

namespace GoBattleSim
{

Party::Party()
{
	m_revive_quota = 0;
	m_revive_policy = 0;
}

Party::Party(const Party &other)
{
	*this = other;
}

Party &Party::operator=(const Party &other)
{
	m_pokemon_count = other.m_pokemon_count;
	for (unsigned i = 0; i < m_pokemon_count; ++i)
	{
		m_pokemon[i] = other.m_pokemon[i];
	}
	m_pokemon_head = m_pokemon + (other.m_pokemon_head - other.m_pokemon);
	m_revive_policy = other.m_revive_policy;
	m_revive_quota = other.m_revive_quota;
	return *this;
}

Party::~Party()
{
	erase_pokemon();
}

void Party::add(const Pokemon *t_pokemon)
{
	if (m_pokemon_count >= MAX_NUM_POKEMON)
	{
		sprintf(err_msg, "too many Pokemon (max %d)", MAX_NUM_POKEMON);
		throw std::runtime_error(err_msg);
	}
	m_pokemon[m_pokemon_count] = *t_pokemon;
	if (m_pokemon_head == nullptr)
	{
		m_pokemon_head = m_pokemon + m_pokemon_count;
	}
	++m_pokemon_count;
}

void Party::update(const Pokemon *t_pokemon)
{
	for (unsigned i = 0; i < m_pokemon_count; ++i)
	{
		if (m_pokemon[i].id == t_pokemon->id)
		{
			m_pokemon[i] = *t_pokemon;
		}
	}
}

void Party::set_revive_policy(int t_policy)
{
	m_revive_policy = t_policy;
}

int Party::get_revive_policy() const
{
	return m_revive_policy;
}

void Party::erase_pokemon()
{
	m_pokemon_count = 0;
	m_pokemon_head = nullptr;
}

unsigned Party::get_pokemon_count() const
{
	return m_pokemon_count;
}

Pokemon *Party::get_pokemon(int t_index)
{
	if (0 <= t_index && t_index < m_pokemon_count)
	{
		return m_pokemon + t_index;
	}
	else
	{
		return m_pokemon_head;
	}
}

const Pokemon *Party::get_pokemon(int t_index) const
{
	if (0 <= t_index && t_index < m_pokemon_count)
	{
		return m_pokemon + t_index;
	}
	else
	{
		return m_pokemon_head;
	}
}

Pokemon **Party::get_all_pokemon(Pokemon **out_first)
{
	for (unsigned i = 0; i < m_pokemon_count; ++i)
	{
		*out_first++ = m_pokemon + i;
	}
	return out_first;
}

void Party::init()
{
	m_pokemon_head = m_pokemon;
	m_revive_quota = m_revive_policy;
}

Pokemon *Party::get_head()
{
	return m_pokemon_head;
}

bool Party::set_head(const Pokemon *t_pokemon)
{
	for (unsigned i = 0; i < m_pokemon_count; ++i)
	{
		if (m_pokemon + i == t_pokemon)
		{
			m_pokemon_head = m_pokemon + i;
			return true;
		}
	}
	return false;
}

bool Party::set_head(int t_index)
{
	if (0 <= t_index && t_index < m_pokemon_count)
	{
		m_pokemon_head = m_pokemon + t_index;
		return true;
	}
	else
	{
		return false;
	}
}

bool Party::revive()
{
	if (m_revive_quota != 0)
	{
		--m_revive_quota;
		return true;
	}
	else
	{
		return false;
	}
}

} // namespace GoBattleSim
