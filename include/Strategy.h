
#ifndef _STRATEGY_H_
#define _STRATEGY_H_

#include "Pokemon.h"
#include "PokemonState.h"

namespace GoBattleSim
{

enum class ActionType : unsigned char
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
	unsigned time{0};
	ActionType type{ActionType::None};
	short value{0};
	unsigned delay{0};

	// workaround for C++ 11
	Action(unsigned _time = 0, ActionType _type = ActionType::None, short _value = 0, int _delay = 0) : time(_time), type(_type), value(_value), delay(_delay) {}
};

struct StrategyInput
{
	// The time when the subject Pokemon will be free
	unsigned time_free;
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

void attacker_fast_only_no_dodge_on_free(const StrategyInput &, Action *);

void attacker_burst_no_dodge_on_free(const StrategyInput &, Action *);

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

const Strategy STRATEGY_ATTACKER_FAST_ONLY_NO_DODGE{
	"ATTACKER_FAST_ONLY_NO_DODGE",
	attacker_fast_only_no_dodge_on_free,
	nullptr,
	nullptr};

const Strategy STRATEGY_ATTACKER_BURST_NO_DODGE{
	"ATTACKER_BURST_NO_DODGE",
	attacker_burst_no_dodge_on_free,
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

const Strategy PVE_STRATEGIES[] = {
	STRATEGY_DEFENDER,
	STRATEGY_ATTACKER_NO_DODGE,
	STRATEGY_ATTACKER_FAST_ONLY_NO_DODGE,
	STRATEGY_ATTACKER_BURST_NO_DODGE,
	STRATEGY_ATTACKER_DODGE_CHARGED,
	STRATEGY_ATTACKER_DODGE_ALL};

const unsigned NUM_PVE_STRATEGIES = sizeof(PVE_STRATEGIES) / sizeof(PVE_STRATEGIES[0]);

} // namespace GoBattleSim

#endif
