
#ifndef _MOVE_H_
#define _MOVE_H_

namespace GoBattleSim
{

struct MoveEffect
{
	double activation_chance{0.0};
	int self_attack_stage_delta{0};
	int self_defense_stage_delta{0};
	int target_attack_stage_delta{0};
	int target_defense_stage_delta{0};
};

struct Move
{
	int poketype{0};
	int power{0};
	int energy{0};
	int duration{0};
	int dws{0};

	MoveEffect effect;
};

} // namespace GoBattleSim

#endif
