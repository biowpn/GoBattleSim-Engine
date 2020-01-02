
#ifndef _MOVE_H_
#define _MOVE_H_

namespace GoBattleSim
{

struct MoveEffect
{
	double activation_chance;
	int self_attack_stage_delta;
	int self_defense_stage_delta;
	int target_attack_stage_delta;
	int target_defense_stage_delta;
};

struct Move
{
	int poketype;
	int power;
	int energy;
	int duration;
	int dws;

	MoveEffect effect;
};

} // namespace GoBattleSim

#endif
