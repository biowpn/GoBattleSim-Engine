
#include "Pokemon.h"

#include "GameMaster.h"

#include <string.h>
#include <stdexcept>
#include <stdio.h>

namespace GoBattleSim
{

int calc_damage(const Pokemon *t_attacker, const Move *t_move, const Pokemon *t_defender, int t_weather)
{
	double multiplier = t_attacker->attack_multiplier;
	if (t_move->poketype == t_attacker->poketype1 || t_move->poketype == t_attacker->poketype2)
	{
		multiplier *= GameMaster::get().stab_multiplier;
	}
	if (GameMaster::get().boosted_weather(t_move->poketype) == t_weather)
	{
		multiplier *= GameMaster::get().wab_multiplier;
	}
	multiplier *= GameMaster::get().effectiveness(t_move->poketype, t_defender->poketype1);
	multiplier *= GameMaster::get().effectiveness(t_move->poketype, t_defender->poketype2);

	return (int)(0.5 * t_attacker->attack / t_defender->defense * t_move->power * multiplier + 1) * t_attacker->clone_multiplier;
}

Pokemon::Pokemon(int t_poketype1, int t_poketype2, double t_attack, double t_defense, int t_max_hp)
	: poketype1(t_poketype1), poketype2(t_poketype2), attack(t_attack), defense(t_defense), max_hp(t_max_hp)
{
}

Pokemon::Pokemon(const Pokemon &other)
{
	*this = other;
}

Pokemon &Pokemon::operator=(const Pokemon &other)
{
	memcpy(this, &other, sizeof(Pokemon));
	cmove = cmoves + (other.cmove - other.cmoves);
	return *this;
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
	if (0 <= t_index && static_cast<unsigned>(t_index) < cmoves_count)
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
		sprintf(err_msg, "too many cmoves (max %d)", MAX_NUM_CMOVES);
		throw std::runtime_error(err_msg);
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

} // namespace GoBattleSim
