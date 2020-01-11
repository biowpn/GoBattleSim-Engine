
#ifndef _MOVE_H_
#define _MOVE_H_

namespace GoBattleSim
{

struct MoveEffect
{
	double activation_chance{0.0};
	int self_atk_delta{0};
	int self_def_delta{0};
	int target_atk_delta{0};
	int target_def_delta{0};

	MoveEffect(double _1 = 0.0, int _2 = 0, int _3 = 0, int _4 = 0, int _5 = 0) : activation_chance(_1), self_atk_delta(_2), self_def_delta(_3), target_atk_delta(_4), target_def_delta(_5)
	{
	}
};

struct Move
{
	int poketype{0};
	int power{0};
	int energy{0};
	int duration{0};
	int dws{0};

	MoveEffect effect;

	Move(int _poketype = 0, int _power = 0, int _energy = 0, int _duration = 0, int _dws = 0, MoveEffect _effect = MoveEffect())
		: poketype(_poketype), power(_power), energy(_energy), duration(_duration), dws(_dws), effect(_effect)
	{
	}
};

} // namespace GoBattleSim

#endif
