
#ifndef _PVP_STRATEGY_H_
#define _PVP_STRATEGY_H_

#include "Strategy.h"
#include "PvPPokemon.h"



typedef struct {
	const Pokemon* subject;
	const Pokemon* enemy;
	
	int subject_hp;
	int enemy_hp;
	
	int subject_energy;
	int enemy_energy;
	
	int subject_shields;
	int enemy_shields;
	
	// Party info

} PvPStrategyInput;



typedef void (*PvPEventResponder)(const PvPStrategyInput&, Action*);



void pvp_basic_on_free(const PvPStrategyInput&, Action*);

void pvp_advance_on_free(const PvPStrategyInput&, Action*);



class PvPStrategy
{
public:
	PvPStrategy(PvPEventResponder=pvp_basic_on_free, PvPEventResponder=nullptr, PvPEventResponder=nullptr);
	
	PvPEventResponder on_free;
	
	PvPEventResponder on_attack;
	
	PvPEventResponder on_switch;
	
};


const PvPStrategy pvp_basic = PvPStrategy(
	pvp_basic_on_free
);

const PvPStrategy pvp_advance = PvPStrategy(
	pvp_advance_on_free
);




#endif