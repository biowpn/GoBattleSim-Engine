
#include "Pokemon.h"

#include "GameMaster.h"

#include <string.h>
#include <stdexcept>
#include <stdio.h>

namespace GoBattleSim
{

int calc_damage(const Pokemon *attacker,
				const Move *move,
				const Pokemon *defender,
				double multiplier)
{
	if (move->poketype == attacker->poketype1 || move->poketype == attacker->poketype2)
	{
		multiplier *= GameMaster::get().stab_multiplier;
	}
	multiplier *= GameMaster::get().effectiveness(move->poketype, defender->poketype1);
	multiplier *= GameMaster::get().effectiveness(move->poketype, defender->poketype2);

	return 0.5 * attacker->attack / defender->defense * move->power * multiplier + 1;
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
	poketype1 = other.poketype1;
	poketype2 = other.poketype2;
	attack = other.attack;
	defense = other.defense;
	max_hp = other.max_hp;
	immortal = other.immortal;

	fmove = other.fmove;
	cmoves_count = other.cmoves_count;
	for (unsigned i = 0; i < other.cmoves_count; ++i)
	{
		cmoves[i] = other.cmoves[i];
	}
	cmove = cmoves + (other.cmove - other.cmoves);

	strategy = other.strategy;

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
