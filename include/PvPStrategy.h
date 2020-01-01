
#ifndef _PVP_STRATEGY_H_
#define _PVP_STRATEGY_H_

#include "Strategy.h"
#include "PvPPokemon.h"

namespace GoBattleSim
{

struct PvPStrategyInput
{
	const Pokemon *subject;
	const Pokemon *enemy;

	int subject_hp;
	int enemy_hp;

	int subject_energy;
	int enemy_energy;

	int subject_shields;
	int enemy_shields;
};

typedef void (*PvPEventResponder)(const PvPStrategyInput &, Action *);

void pvp_basic_on_free(const PvPStrategyInput &, Action *);

void pvp_advance_on_free(const PvPStrategyInput &, Action *);

struct PvPStrategy
{
	PvPEventResponder on_free;

	PvPEventResponder on_attack;

	PvPEventResponder on_switch;
};

const PvPStrategy pvp_basic{
	pvp_basic_on_free,
	nullptr,
	nullptr};

const PvPStrategy pvp_advance{
	pvp_advance_on_free,
	nullptr,
	nullptr};

} // namespace GoBattleSim

#endif
