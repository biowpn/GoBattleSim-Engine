
#include "Pokemon.h"

#include <string.h>
#include <stdexcept>

namespace GoBattleSim
{

int Pokemon::instance_count = 0;

int calc_damage(const Pokemon *t_attacker, const Move *t_move, const Pokemon *t_defender, int t_weather)
{
	double multiplier = t_attacker->attack_multiplier;
	if (t_move->poketype == t_attacker->poketype1 || t_move->poketype == t_attacker->poketype2)
	{
		multiplier *= GameMaster::same_type_attack_bonus_multiplier;
	}
	if (GameMaster::get_type_boosted_weather(t_move->poketype) == t_weather)
	{
		multiplier *= GameMaster::weather_attack_bonus_multiplier;
	}
	multiplier *= GameMaster::get_effectiveness(t_move->poketype, t_defender->poketype1);
	multiplier *= GameMaster::get_effectiveness(t_move->poketype, t_defender->poketype2);

	return (int)(0.5 * t_attacker->attack / t_defender->defense * t_move->power * multiplier + 1) * t_attacker->clone_multiplier;
}

Pokemon::Pokemon(int t_poketype1, int t_poketype2, double t_attack, double t_defense, int t_max_hp)
	: poketype1(t_poketype1), poketype2(t_poketype2), attack(t_attack), defense(t_defense), max_hp(t_max_hp)
{
	id = instance_count++;
}

Pokemon::Pokemon(const Pokemon &other)
{
	id = other.id;
	poketype1 = other.poketype1;
	poketype2 = other.poketype2;
	attack = other.attack;
	defense = other.defense;
	max_hp = other.max_hp;

	attack_multiplier = other.attack_multiplier;
	clone_multiplier = other.clone_multiplier;

	fmove = other.fmove;
	cmoves_count = 0;
	add_fmove(&other.fmove);
	for (int i = 0; i < other.cmoves_count; ++i)
	{
		add_cmove(&other.cmoves[i]);
	}
	cmove = cmoves + (other.cmove - other.cmoves);
}

Pokemon::~Pokemon()
{
	erase_cmoves();
}

const Move *Pokemon::get_fmove(int) const
{
	return &fmove;
}

void Pokemon::add_fmove(const Move *t_move)
{
	if (t_move != nullptr)
	{
		fmove = *t_move;
	}
}

const Move *Pokemon::get_cmove(int t_index) const
{
	if (0 <= t_index && t_index < cmoves_count)
	{
		return cmoves + t_index;
	}
	else
	{
		return cmove;
	}
}

void Pokemon::add_cmove(const Move *t_move)
{
	if (t_move == nullptr)
	{
		return;
	}
	if (cmoves_count >= MAX_NUM_CMOVES)
	{
		throw std::runtime_error("too many cmoves");
	}
	cmoves[cmoves_count] = *t_move;
	if (cmove == nullptr)
	{
		cmove = cmoves + cmoves_count;
	}
	++cmoves_count;
}

void Pokemon::erase_cmoves()
{
	cmoves_count = 0;
	cmove = nullptr;
}

bool Pokemon::has_attr(const char *t_name)
{
	return search_bool_member(t_name) || search_int_member(t_name) || search_double_member(t_name);
}

double Pokemon::get_attr(const char *t_name)
{
	bool *bool_member_ptr = search_bool_member(t_name);
	if (bool_member_ptr)
	{
		return *bool_member_ptr;
	}
	int *int_member_ptr = search_int_member(t_name);
	if (int_member_ptr)
	{
		return *int_member_ptr;
	}
	double *double_member_ptr = search_double_member(t_name);
	if (double_member_ptr)
	{
		return *double_member_ptr;
	}
	return 0;
}

void Pokemon::set_attr(const char *t_name, double t_value)
{
	bool *bool_member_ptr = search_bool_member(t_name);
	if (bool_member_ptr)
	{
		*bool_member_ptr = t_value;
		return;
	}
	int *int_member_ptr = search_int_member(t_name);
	if (int_member_ptr)
	{
		*int_member_ptr = t_value;
		return;
	}
	double *double_member_ptr = search_double_member(t_name);
	if (double_member_ptr)
	{
		*double_member_ptr = t_value;
		return;
	}
}

bool *Pokemon::search_bool_member(const char *t_name)
{
	if (strcmp(t_name, "immortal") == 0)
		return &immortal;
	else
		return nullptr;
}

int *Pokemon::search_int_member(const char *t_name)
{
	if (strcmp(t_name, "id") == 0)
		return &id;
	else if (strcmp(t_name, "poketype1") == 0)
		return &poketype1;
	else if (strcmp(t_name, "poketype2") == 0)
		return &poketype2;
	else if (strcmp(t_name, "max_hp") == 0)
		return &max_hp;
	else if (strcmp(t_name, "cmoves_count") == 0)
		return &cmoves_count;
	else if (strcmp(t_name, "clone_multiplier") == 0)
		return &clone_multiplier;
	else
		return nullptr;
}

double *Pokemon::search_double_member(const char *t_name)
{
	if (strcmp(t_name, "attack_multiplier") == 0)
		return &attack_multiplier;
	else if (strcmp(t_name, "attack") == 0)
		return &attack;
	else if (strcmp(t_name, "defense") == 0)
		return &defense;
	else
		return nullptr;
}

} // namespace GoBattleSim
