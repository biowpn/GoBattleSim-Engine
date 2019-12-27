
#include "Party.h"

Party::Party()
{
	m_pokemon = nullptr;
	m_pokemon_count = 0;
	m_pokemon_count_max = 0;
	m_pokemon_head = 0;
	m_revive_policy = 0;
	m_revive_policy_init = 0;
}

Party::Party(const Party &other)
{
	m_pokemon_head = other.m_pokemon_head;
	m_revive_policy = other.m_revive_policy;
	m_revive_policy_init = other.m_revive_policy_init;

	m_pokemon = nullptr;
	m_pokemon_count = 0;
	m_pokemon_count_max = 0;
	for (int i = 0; i < other.m_pokemon_count; ++i)
	{
		add(other.m_pokemon[i]);
	}
}

Party::~Party()
{
	erase_pokemon();
}

void Party::add(Pokemon *t_pokemon)
{
	if (m_pokemon_count >= m_pokemon_count_max)
	{
		m_pokemon_count_max += 2;
		Pokemon **pokemon_temp = new Pokemon *[m_pokemon_count_max];
		for (int i = 0; i < m_pokemon_count; ++i)
		{
			pokemon_temp[i] = m_pokemon[i];
		}
		if (m_pokemon)
		{
			delete[] m_pokemon;
		}
		m_pokemon = pokemon_temp;
	}
	m_pokemon[m_pokemon_count++] = new Pokemon(*t_pokemon);
}

void Party::update(Pokemon *t_pokemon)
{
	for (int i = 0; i < m_pokemon_count; ++i)
	{
		if (m_pokemon[i]->id == t_pokemon->id)
		{
			delete m_pokemon[i];
			m_pokemon[i] = new Pokemon(*t_pokemon);
		}
	}
}

void Party::erase_pokemon()
{
	if (m_pokemon && m_pokemon_count > 0)
	{
		for (int i = 0; i < m_pokemon_count; ++i)
		{
			delete m_pokemon[i];
		}
		delete[] m_pokemon;
	}
	m_pokemon = nullptr;
	m_pokemon_count = 0;
	m_pokemon_count_max = 0;
}

bool Party::has_attr(const char *t_name)
{
	return search_int_member(t_name);
}

int Party::get_attr(const char *t_name)
{
	int *int_member_ptr = search_int_member(t_name);
	if (int_member_ptr)
	{
		return *int_member_ptr;
	}
	return 0;
}

void Party::set_attr(const char *t_name, int t_value)
{
	int *int_member_ptr = search_int_member(t_name);
	if (int_member_ptr)
	{
		*int_member_ptr = t_value;
	}
}

int *Party::search_int_member(const char *t_name)
{
	if (strcmp(t_name, "revive_policy") == 0)
		return &m_revive_policy_init;
	else if (strcmp(t_name, "pokemon_count") == 0)
		return &m_pokemon_count;
	else
		return nullptr;
}

int Party::get_pokemon_count() const
{
	return m_pokemon_count;
}

Pokemon *Party::get_pokemon(int t_index)
{
	return m_pokemon[t_index];
}

void Party::get_all_pokemon(Pokemon **t_array)
{
	for (int i = 0; i < m_pokemon_count; ++i)
	{
		t_array[i] = m_pokemon[i];
	}
}

void Party::init()
{
	for (int i = 0; i < m_pokemon_count; ++i)
	{
		m_pokemon[i]->init();
	}
	m_pokemon_head = 0;
	m_revive_policy = m_revive_policy_init;
}

void Party::heal()
{
	for (int i = 0; i < m_pokemon_count; ++i)
	{
		m_pokemon[i]->heal();
	}
	m_pokemon_head = 0;
}

Pokemon *Party::get_head()
{
	return m_pokemon[m_pokemon_head];
}

bool Party::set_head(Pokemon *t_pokemon)
{
	for (int i = 0; i < m_pokemon_count; ++i)
	{
		if (m_pokemon[i] == t_pokemon)
		{
			m_pokemon_head = i;
			return true;
		}
	}
	return false;
}

bool Party::set_head(int t_index)
{
	if (0 <= t_index && t_index < m_pokemon_count)
	{
		m_pokemon_head = t_index;
		return true;
	}
	else
	{
		return false;
	}
}

bool Party::set_head_to_next()
{
	for (int i = (m_pokemon_head + 1) % m_pokemon_count; i != m_pokemon_head; i = (i + 1) % m_pokemon_count)
	{
		if (m_pokemon[i]->is_alive())
		{
			m_pokemon_head = i;
			return true;
		}
	}
	return false;
}

bool Party::revive()
{
	if (m_revive_policy != 0)
	{
		heal();
		--m_revive_policy;
		return true;
	}
	else
	{
		return false;
	}
}
