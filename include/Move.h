
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

class Move
{
public:
	// Interface functions
	bool has_attr(const char *);
	int get_attr(const char *);
	void set_attr(const char *, int);
	// End of interface functions

	int poketype;
	int power;
	int energy;
	int duration;
	int dws;

	MoveEffect effect;

protected:
	int *search_int_member(const char *);
};

} // namespace GoBattleSim

#endif
