
#ifndef _STRATEGY_H_
#define _STRATEGY_H_

#include "Party.h"
#include "GameMaster.h"

namespace GoBattleSim
{

enum class ActionType
{
	None,
	Wait,
	Fast,
	Charged,
	Dodge,
	Switch
};

struct Action
{
	ActionType type;
	int delay;
	int value;
	int time;
};

struct PokemonState
{
	bool active{false};
	bool immortal{false};
	int max_hp{0};
	int hp{0};
	int energy{0};
	int damage_reduction_expiry{0};
	int tdo;
	int tdo_fast;
	int duration;
	int num_deaths;
	int num_fmoves_used;
	int num_cmoves_used;

	void init();
	void heal();
	bool is_alive() const;
	void charge(int);
	void hurt(int);
	void attribute_damage(int, bool);
};

struct StrategyInput
{
	// The time when the subject Pokemon will be free
	int time_free;
	// The subject Pokemon
	const Pokemon *subject;
	// The primary enemy Pokemon
	const Pokemon *enemy;
	// The subject Pokemon' state
	const PokemonState *subject_state;
	// The enemy's state
	const PokemonState *enemy_state;
	// The subject player's action that is being executed
	Action subject_action;
	// The enemy player's action that is being executed
	Action enemy_action;

	// A random number passed by the controller. If the strategy involves some sort of randomness, use this number
	int random_number;
	// The weather
	int weather;
};

typedef void (*EventResponder)(const StrategyInput &, Action *);

struct Strategy
{
	// A name for the strategy
	char name[32];

	// Called when the subject is free.
	// The returned Action will be executed immediately.
	// This must not be NULL.
	EventResponder on_free;

	// Called right before the subject's buffer action is cleared and executed.
	// The returned Action will be the new buffer action.
	EventResponder on_clear;

	// Called when the enemy announces a new attack (when the attack text appears).
	// The returned Action will be the new buffer action.
	EventResponder on_attack;
};

// Built-in strategies

void defender_on_clear(const StrategyInput &, Action *);

void attacker_no_dodge_on_free(const StrategyInput &, Action *);

void attacker_dodge_charged_on_free(const StrategyInput &, Action *);
void attacker_dodge_charged_on_attack(const StrategyInput &, Action *);

void attacker_dodge_all_on_free(const StrategyInput &, Action *);
void attacker_dodge_all_on_attack(const StrategyInput &, Action *);

const Strategy STRATEGY_DEFENDER{
	"DEFENDER",
	attacker_no_dodge_on_free,
	defender_on_clear,
	nullptr};

const Strategy STRATEGY_ATTACKER_NO_DODGE{
	"ATTACKER_NO_DODGE",
	attacker_no_dodge_on_free,
	nullptr,
	nullptr};

const Strategy STRATEGY_ATTACKER_DODGE_CHARGED{
	"ATTACKER_DODGE_CHARGED",
	attacker_dodge_charged_on_free,
	nullptr,
	attacker_dodge_charged_on_attack};

const Strategy STRATEGY_ATTACKER_DODGE_ALL{
	"ATTACKER_DODGE_ALL",
	attacker_dodge_all_on_free,
	nullptr,
	attacker_dodge_all_on_attack};

const Strategy BUILT_IN_STRATEGIES[] = {
	STRATEGY_DEFENDER,
	STRATEGY_ATTACKER_NO_DODGE,
	STRATEGY_ATTACKER_DODGE_CHARGED,
	STRATEGY_ATTACKER_DODGE_ALL};

const unsigned NUM_STRATEGIES = sizeof(BUILT_IN_STRATEGIES) / sizeof(BUILT_IN_STRATEGIES[0]);

} // namespace GoBattleSim

#endif