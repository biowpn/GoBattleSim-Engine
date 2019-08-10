
#ifndef _STRATEGY_H_
#define _STRATEGY_H_

#include "Party.h"
#include "GameMaster.h"

#include <ostream>

enum ActionType {
	atype_None,
	atype_Wait,
	atype_Fast,
	atype_Charged,
	atype_Dodge,
	atype_Switch
};


class Action
{
public:
	Action(ActionType=atype_None, int=0, int=0);

	ActionType type;
	int delay;
	int value;
	int time;
};

std::ostream& operator<<(std::ostream&, const Action&);



typedef struct {
	// The time when the subject Pokemon will be free
	int time;
	// The subject Pokemon
	const Pokemon* subject;
	// The primary enemy Pokemon
	const Pokemon* enemy;
	// The subject player's action that is being executed
	Action subject_action;
	// The enemy player's action that is being executed
	Action enemy_action;
	
	// A random number passed by the controller. If the strategy involves some sort of randomness, use this number
	int random_number;
	// The weather
	int weather;
	
} StrategyInput;


typedef void (*EventResponder)(const StrategyInput&, Action*);



class Strategy {
public:
	Strategy(EventResponder=nullptr, EventResponder=nullptr, EventResponder=nullptr);
	
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

void defender_on_clear(const StrategyInput&, Action*);

void attacker_no_dodge_on_free(const StrategyInput&, Action*);

void attacker_dodge_charged_on_free(const StrategyInput&, Action*);
void attacker_dodge_charged_on_attack(const StrategyInput&, Action*);

void attacker_dodge_all_on_free(const StrategyInput&, Action*);
void attacker_dodge_all_on_attack(const StrategyInput&, Action*);


const Strategy STRATEGY_DEFENDER(
	attacker_no_dodge_on_free,
	defender_on_clear
);

const Strategy STRATEGY_ATTACKER_NO_DODGE(attacker_no_dodge_on_free);

const Strategy STRATEGY_ATTACKER_DODGE_CHARGED(
	attacker_dodge_charged_on_free, 
	nullptr, 
	attacker_dodge_charged_on_attack
);
	
const Strategy STRATEGY_ATTACKER_DODGE_ALL(
	attacker_dodge_all_on_free,
	nullptr,
	attacker_dodge_all_on_attack
);


const int NUM_STRATEGIES = 4;
const Strategy BUILT_IN_STRATEGIES[NUM_STRATEGIES] = {
	STRATEGY_DEFENDER,
	STRATEGY_ATTACKER_NO_DODGE,
	STRATEGY_ATTACKER_DODGE_CHARGED,
	STRATEGY_ATTACKER_DODGE_ALL
};





#endif