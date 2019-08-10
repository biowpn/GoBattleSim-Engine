
#include "Player.h"

int Player::instance_count = 0;

Player::Player()
{
	id = instance_count++;
	team = 0;
	
	m_parties = nullptr;
	m_parties_count = 0;
	m_parties_count_max = 0;
	m_party_head = 0;
}


Player::Player(const Player& other)
{
	id = other.id;
	team = other.team;
	strategy = other.strategy;
	m_party_head = other.m_party_head;
	
	m_parties = nullptr;
	m_parties_count = 0;
	m_parties_count_max = 0;
	for (int i = 0; i < other.m_parties_count; ++i)
	{
		add(other.m_parties[i]);
	}
}


Player::~Player()
{
	erase_parties();
}


Party* Player::get_party(int t_index)
{
	return m_parties[t_index];
}


void Player::add(Party* t_party)
{
	if (m_parties_count >= m_parties_count_max)
	{
		m_parties_count_max += 2;
		Party** parties_temp = new Party*[m_parties_count_max];
		for (int i = 0; i < m_parties_count_max; ++i)
		{
			parties_temp[i] = i < m_parties_count ? m_parties[i] : nullptr;
		}
		if (m_parties)
		{
			delete[] m_parties;
		}
		m_parties = parties_temp;
	}
	m_parties[m_parties_count++] = new Party(*t_party);
}



void Player::erase_parties()
{
	if (m_parties && m_parties_count > 0)
	{
		for (int i = 0; i < m_parties_count; ++i)
		{
			delete m_parties[i];
		}
		delete[] m_parties;
	}
	m_parties = nullptr;
	m_parties_count = 0;
	m_parties_count_max = 0;
}




bool Player::has_attr(const char* t_name)
{
	return search_int_member(t_name);
}


int Player::get_attr(const char* t_name)
{
	int* int_member_ptr = search_int_member(t_name);
	if (int_member_ptr)
	{
		return *int_member_ptr;
	}
	return 0;
}


void Player::set_attr(const char* t_name, int t_value)
{
	int* int_member_ptr = search_int_member(t_name);
	if (int_member_ptr)
	{
		*int_member_ptr = t_value;
		return;
	}
}


int* Player::search_int_member(const char* t_name)
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



void Player::update(Pokemon* t_pokemon)
{
	for (int i = 0; i < m_parties_count; ++i)
	{
		m_parties[i]->update(t_pokemon);
	}
}


void Player::set_attack_multiplier(double t_attack_multiplier)
{
	for (int i = 0; i < m_parties_count; ++i)
	{
		for (int j = 0; j < m_parties[i]->get_pokemon_count(); ++j)
		{
			m_parties[i]->get_pokemon(j)->attack_multiplier = t_attack_multiplier;
		}
	}
}


void Player::set_clone_multiplier(int t_clone_multiplier)
{
	for (int i = 0; i < m_parties_count; ++i)
	{
		for (int j = 0; j < m_parties[i]->get_pokemon_count(); ++j)
		{
			m_parties[i]->get_pokemon(j)->clone_multiplier = t_clone_multiplier;
		}
	}
}


void Player::set_strategy(int t_strategy_index)
{
	if (0 <= t_strategy_index && t_strategy_index < NUM_STRATEGIES)
	{
		set_strategy(BUILT_IN_STRATEGIES[t_strategy_index]);
	}
}


void Player::set_strategy(const Strategy& t_strategy)
{
	strategy = t_strategy;
}



int Player::get_pokemon_count() const
{
	int count = 0;
	for (int i = 0; i < m_parties_count; ++i)
	{
		count += m_parties[i]->get_pokemon_count();
	}
	return count;
}


void Player::get_all_pokemon(Pokemon** t_array) const
{
	for (int i = 0; i < m_parties_count; ++i)
	{
		int cur_party_pokemon_count = m_parties[i]->get_pokemon_count();
		m_parties[i]->get_all_pokemon(t_array);
		t_array += cur_party_pokemon_count;
	}
}


Party* Player::get_head_party()
{
	return m_parties[m_party_head];
}


void Player::init()
{
	for (int i = 0; i < m_parties_count; ++i)
	{
		m_parties[i]->init();
	}
	m_party_head = 0;
}


void Player::heal()
{
	for (int i = 0; i < m_parties_count; ++i)
	{
		m_parties[i]->heal();
	}
	m_party_head = 0;
}


Pokemon* Player::get_head()
{
	return m_parties[m_party_head]->get_head();
}


bool Player::set_head(Pokemon* t_pokemon)
{
	return m_parties[m_party_head]->set_head(t_pokemon);
}


bool Player::choose_next_pokemon()
{
	Party* cur_party = m_parties[m_party_head];
	return cur_party->set_head_to_next();
}


bool Player::revive_current_party()
{
	Party* cur_party = m_parties[m_party_head];
	return cur_party->revive();
}


bool Player::choose_next_party()
{
	if (m_party_head + 1 < m_parties_count)
	{
		++m_party_head;
		return true;
	}
	else
	{
		return false;
	}
}


int Player::get_tdo() const
{
	int tdo = 0;
	for (int i = 0; i < m_parties_count; ++i)
	{
		for (int j = 0; j < m_parties[i]->get_pokemon_count(); ++j)
		{
			tdo += m_parties[i]->get_pokemon(j)->tdo;
		}
	}
	return tdo;
}


int Player::get_max_hp() const
{
	int max_hp = 0;
	for (int i = 0; i < m_parties_count; ++i)
	{
		for (int j = 0; j < m_parties[i]->get_pokemon_count(); ++j)
		{
			max_hp += m_parties[i]->get_pokemon(j)->max_hp;
		}
	}
	return max_hp;
}


int Player::get_num_deaths() const
{
	int num_deaths = 0;
	for (int i = 0; i < m_parties_count; ++i)
	{
		for (int j = 0; j < m_parties[i]->get_pokemon_count(); ++j)
		{
			num_deaths += m_parties[i]->get_pokemon(j)->num_deaths;
		}
	}
	return num_deaths;
}






