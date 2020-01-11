
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
	// A name for the strategy
	char name[32];

	// Called when the subject is free.
	// The returned Action will be executed immediately.
	// This must not be NULL.
	PvPEventResponder on_free;

	// Called when the opponent is using a Charged Attack.
	// Set the action type to ActioType::Dodge to indicate using a shield.
	PvPEventResponder on_attack;

	// Called when a new opponent enters the arena,
	// including when the game begins.
	PvPEventResponder on_switch;
};

const PvPStrategy STRATEGY_PVP_BASIC{
	"PVP_BASIC",
	pvp_basic_on_free,
	nullptr,
	nullptr};

const PvPStrategy STRATEGY_PVP_ADVANCE{
	"PVP_ADVANCE",
	pvp_advance_on_free,
	nullptr,
	nullptr};

const PvPStrategy PVP_STRATEGIES[] = {
	STRATEGY_PVP_BASIC,
	STRATEGY_PVP_ADVANCE};

const unsigned NUM_PVP_STRATEGIES = sizeof(PVP_STRATEGIES) / sizeof(PVP_STRATEGIES[0]);

} // namespace GoBattleSim

#endif
