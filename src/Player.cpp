
#include "Player.h"

#include <string.h>
#include <stdexcept>

namespace GoBattleSim
{

int Player::instance_count = 0;

Player::Player()
{
	id = instance_count++;
	team = 0;

	m_parties_count = 0;
	m_party_head = nullptr;
}

Player::Player(const Player &other)
{
	*this = other;
}

Player &Player::operator=(const Player &other)
{
	m_parties_count = other.m_parties_count;
	for (int i = 0; i < m_parties_count; ++i)
	{
		m_parties[i] = other.m_parties[i];
	}
	m_party_head = m_parties + (other.m_party_head - other.m_parties);
	id = other.id;
	team = other.team;
	memcpy(&strategy, &other.strategy, sizeof(Strategy));
	m_attack_multiplier = other.m_attack_multiplier;
	m_clone_multiplier = other.m_clone_multiplier;
	return *this;
}

Player::~Player()
{
	erase_parties();
}

Party *Player::get_party(int t_index)
{
	if (0 <= t_index && t_index < m_parties_count)
	{
		return m_parties + t_index;
	}
	else
	{
		return m_party_head;
	}
}

const Party *Player::get_party(int t_index) const
{
	if (0 <= t_index && t_index < m_parties_count)
	{
		return m_parties + t_index;
	}
	else
	{
		return m_party_head;
	}
}

int Player::get_parties_count() const
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

void Player::update(const Pokemon *t_pokemon)
{
	for (int i = 0; i < m_parties_count; ++i)
	{
		m_parties[i].update(t_pokemon);
	}
}

void Player::set_attack_multiplier(double t_attack_multiplier)
{
	for (int i = 0; i < m_parties_count; ++i)
	{
		for (int j = 0; j < m_parties[i].get_pokemon_count(); ++j)
		{
			m_parties[i].get_pokemon(j)->attack_multiplier = t_attack_multiplier;
		}
	}
	m_attack_multiplier = t_attack_multiplier;
}

double Player::get_attack_multiplier() const
{
	return m_attack_multiplier;
}

void Player::set_clone_multiplier(int t_clone_multiplier)
{
	for (int i = 0; i < m_parties_count; ++i)
	{
		for (int j = 0; j < m_parties[i].get_pokemon_count(); ++j)
		{
			m_parties[i].get_pokemon(j)->clone_multiplier = t_clone_multiplier;
		}
	}
	m_clone_multiplier = t_clone_multiplier;
}

int Player::get_clone_multiplier() const
{
	return m_clone_multiplier;
}

void Player::set_strategy(int t_strategy_index)
{
	if (0 <= t_strategy_index && t_strategy_index < NUM_STRATEGIES)
	{
		set_strategy(BUILT_IN_STRATEGIES[t_strategy_index]);
	}
}

void Player::set_strategy(const Strategy &t_strategy)
{
	strategy = t_strategy;
}

int Player::get_pokemon_count() const
{
	int count = 0;
	for (int i = 0; i < m_parties_count; ++i)
	{
		count += m_parties[i].get_pokemon_count();
	}
	return count;
}

Pokemon **Player::get_all_pokemon(Pokemon **out_first)
{
	for (int i = 0; i < m_parties_count; ++i)
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
	for (int i = 0; i < m_parties_count; ++i)
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
	else
	{
		return m_party_head->set_head(t_pokemon);
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
		return m_party_head->revive();
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

bool Player::has_attr(const char *t_name)
{
	return search_int_member(t_name);
}

int Player::get_attr(const char *t_name)
{
	int *int_member_ptr = search_int_member(t_name);
	if (int_member_ptr)
	{
		return *int_member_ptr;
	}
	return 0;
}

void Player::set_attr(const char *t_name, int t_value)
{
	int *int_member_ptr = search_int_member(t_name);
	if (int_member_ptr)
	{
		*int_member_ptr = t_value;
		return;
	}
}

int *Player::search_int_member(const char *t_name)
{
	if (strcmp(t_name, "id") == 0)
		return &id;
	else if (strcmp(t_name, "team") == 0)
		return &team;
	else if (strcmp(t_name, "parties_count") == 0)
		return &m_parties_count;
	else
		return nullptr;
}

} // namespace GoBattleSim
