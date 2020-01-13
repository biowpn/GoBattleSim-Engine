
#include "Player.h"

#include "GameMaster.h"

#include <string.h>
#include <stdexcept>
#include <stdio.h>

namespace GoBattleSim
{

Player::Player(const Player &other)
{
	*this = other;
}

Player &Player::operator=(const Player &other)
{
	m_parties_count = other.m_parties_count;
	for (unsigned i = 0; i < m_parties_count; ++i)
	{
		m_parties[i] = other.m_parties[i];
	}
	m_party_head = m_parties + (other.m_party_head - other.m_parties);
	team = other.team;
	memcpy(&strategy, &other.strategy, sizeof(Strategy));
	attack_multiplier = other.attack_multiplier;
	clone_multiplier = other.clone_multiplier;
	return *this;
}

Player::~Player()
{
	erase_parties();
}

Party *Player::get_party(unsigned t_index)
{
	if (t_index < m_parties_count)
	{
		return m_parties + t_index;
	}
	else
	{
		return m_party_head;
	}
}

const Party *Player::get_party(unsigned t_index) const
{
	if (t_index < m_parties_count)
	{
		return m_parties + t_index;
	}
	else
	{
		return m_party_head;
	}
}

unsigned Player::get_parties_count() const
{
	return m_parties_count;
}

void Player::add(const Party *t_party)
{
	if (m_parties_count >= MAX_NUM_PARTIES)
	{
		sprintf(err_msg, "too many parties (max %d)", MAX_NUM_PARTIES);
		throw std::runtime_error(err_msg);
	}
	m_parties[m_parties_count] = *t_party;
	if (m_party_head == nullptr)
	{
		m_party_head = m_parties + m_parties_count;
	}
	++m_parties_count;
}

void Player::erase_parties()
{
	m_parties_count = 0;
	m_party_head = nullptr;
}

void Player::set_strategy(const Strategy &t_strategy)
{
	strategy = t_strategy;
	for (unsigned i = 0; i < m_parties_count; ++i)
	{
		for (unsigned j = 0; j < m_parties[i].get_pokemon_count(); ++j)
		{
			m_parties[i].get_pokemon(j)->strategy = &t_strategy;
		}
	}
}

unsigned Player::get_pokemon_count() const
{
	unsigned count = 0;
	for (unsigned i = 0; i < m_parties_count; ++i)
	{
		count += m_parties[i].get_pokemon_count();
	}
	return count;
}

Pokemon **Player::get_all_pokemon(Pokemon **out_first)
{
	for (unsigned i = 0; i < m_parties_count; ++i)
	{
		out_first = m_parties[i].get_all_pokemon(out_first);
	}
	return out_first;
}

Party *Player::get_head_party()
{
	return m_party_head;
}

void Player::init()
{
	for (unsigned i = 0; i < m_parties_count; ++i)
	{
		m_parties[i].init();
	}
	reset_head_party();
}

void Player::reset_head_party()
{
	m_party_head = m_parties;
}

Pokemon *Player::get_head()
{
	if (m_party_head == nullptr)
	{
		return nullptr;
	}
	else
	{
		return m_party_head->get_head();
	}
}

const Pokemon *Player::get_head() const
{
	if (m_party_head == nullptr)
	{
		return nullptr;
	}
	else
	{
		return m_party_head->get_head();
	}
}

bool Player::set_head(const Pokemon *t_pokemon)
{
	if (m_party_head == nullptr)
	{
		return false;
	}
	else if (m_party_head->set_head(t_pokemon))
	{
		if (t_pokemon->strategy != nullptr)
		{
			strategy = *t_pokemon->strategy;
		}
		return true;
	}
	else
	{
		return false;
	}
}

bool Player::revive_current_party()
{
	if (m_party_head == nullptr)
	{
		return false;
	}
	else
	{
		return m_party_head->revive_policy;
	}
}

bool Player::set_head_party_to_next()
{
	if (++m_party_head < m_parties + m_parties_count)
	{
		return true;
	}
	else
	{
		--m_party_head;
		return false;
	}
}

} // namespace GoBattleSim
