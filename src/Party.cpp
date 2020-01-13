
#include "Party.h"

#include "GameMaster.h"

#include <string.h>
#include <stdexcept>
#include <stdio.h>

namespace GoBattleSim
{

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
	revive_policy = other.revive_policy;
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

void Party::erase_pokemon()
{
	m_pokemon_count = 0;
	m_pokemon_head = nullptr;
}

unsigned Party::get_pokemon_count() const
{
	return m_pokemon_count;
}

Pokemon *Party::get_pokemon(unsigned t_index)
{
	if (t_index < m_pokemon_count)
	{
		return m_pokemon + t_index;
	}
	else
	{
		return m_pokemon_head;
	}
}

const Pokemon *Party::get_pokemon(unsigned t_index) const
{
	if (t_index < m_pokemon_count)
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

} // namespace GoBattleSim
