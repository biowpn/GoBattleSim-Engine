
#ifndef _POKEMON_H_
#define _POKEMON_H_

#include <string.h>

#include "Move.h"
#include "GameMaster.h"

class Pokemon
{
public:
	Pokemon(int, int, double, double, int);
	Pokemon(const Pokemon &);
	~Pokemon();

	// Interface functions
	Move *get_fmove(int) const;
	void add_fmove(const Move *);
	Move *get_cmove(int) const;
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
	int poketype1;
	int poketype2;
	double attack;
	double defense;
	int max_hp;

	Move *fmove;
	Move *cmove;
	Move **cmoves;
	int cmoves_count;
	int cmoves_count_max;

	// More members
	bool active;
	bool immortal;
	int hp;
	int energy;
	double attack_multiplier;
	int clone_multiplier;
	int damage_reduction_expired_time;

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

#endif