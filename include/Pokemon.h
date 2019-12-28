
#ifndef _POKEMON_H_
#define _POKEMON_H_

#include "Move.h"
#include "GameMaster.h"

namespace GoBattleSim
{

constexpr unsigned MAX_NUM_CMOVES = 3;

class Pokemon
{
public:
	Pokemon() = default;
	Pokemon(int, int, double, double, int);
	Pokemon(const Pokemon &);
	~Pokemon();

	// Interface functions
	const Move *get_fmove(int) const;
	void add_fmove(const Move *);
	const Move *get_cmove(int) const;
	void add_cmove(const Move *);
	void erase_cmoves();
	bool has_attr(const char *);
	void set_attr(const char *, double);
	double get_attr(const char *);
	// End of Interface functions

	// Battle functions
	void init();
	void heal();
	bool is_alive() const;
	void charge(int);
	void hurt(int);
	void attribute_damage(int, bool);

	// Members
	int id;
	int poketype1{0};
	int poketype2{0};
	double attack{0};
	double defense{0};
	int max_hp{0};

	Move fmove;
	Move *cmove{nullptr};
	Move cmoves[MAX_NUM_CMOVES];
	int cmoves_count{0};

	// More members
	bool active{false};
	bool immortal{false};
	int hp{0};
	int energy{0};
	double attack_multiplier{1};
	int clone_multiplier{1};
	int damage_reduction_expired_time{0};

	// Statistical members
	int tdo;
	int tdo_fast;
	int duration;
	int num_deaths;
	int num_fmoves_used;
	int num_cmoves_used;

protected:
	static int instance_count;

	bool *search_bool_member(const char *);
	int *search_int_member(const char *);
	double *search_double_member(const char *);
};

// Damage calculation as a public function
int calc_damage(const Pokemon *, const Move *, const Pokemon *, int);

} // namespace GoBattleSim

#endif