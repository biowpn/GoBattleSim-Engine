
#ifndef _PLAYER_H_
#define _PLAYER_H_

#include "Party.h"
#include "Strategy.h"

namespace GoBattleSim
{

constexpr unsigned MAX_NUM_PARTIES = 5;

class Player
{
public:
	Player();
	Player(const Player &);
	~Player();

	// Interface functions
	Party *get_party(int);
	const Party *get_party(int) const;
	void add(const Party *);
	void erase_parties();
	void set_attack_multiplier(double);
	void set_clone_multiplier(int);
	void set_strategy(int);
	void set_strategy(const Strategy &);
	bool has_attr(const char *);
	void set_attr(const char *, int);
	int get_attr(const char *);
	// End of Interface functions

	void update(const Pokemon *);

	// Batle functions
	void init();
	void heal();
	Party *get_head_party();
	int get_pokemon_count() const;
	// same with Party::get_all_pokemon, get only the addresses
	Pokemon **get_all_pokemon(Pokemon **out_first);

	Pokemon *get_head();
	bool set_head(const Pokemon *);
	bool choose_next_pokemon();
	bool revive_current_party();
	bool choose_next_party();

	// Battle statistic functions
	int get_tdo() const;
	int get_max_hp() const;
	int get_num_deaths() const;

	// Battle state variables
	int id;
	int team;
	Strategy strategy;

private:
	static int instance_count;

	int m_parties_count{0};
	Party m_parties[MAX_NUM_PARTIES];
	Party *m_party_head{nullptr};

	int *search_int_member(const char *);
};

} // namespace GoBattleSim

#endif