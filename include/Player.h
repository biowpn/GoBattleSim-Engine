
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

	Party *get_party(int);
	const Party *get_party(int) const;
	int get_parties_count() const;
	void add(const Party *);
	void erase_parties();
	void set_attack_multiplier(double);
	double get_attack_multiplier() const;
	void set_clone_multiplier(int);
	int get_clone_multiplier() const;
	void set_strategy(int);
	void set_strategy(const Strategy &);

	void update(const Pokemon *);
	Party *get_head_party();
	int get_pokemon_count() const;
	// same with Party::get_all_pokemon, get only the addresses
	Pokemon **get_all_pokemon(Pokemon **out_first);

	Pokemon *get_head();
	const Pokemon *get_head() const;
	bool set_head(const Pokemon *);

	void init();
	void reset_head_party();

	// only ask whether to revive,
	// the revive is done in battle
	bool revive_current_party();

	// set head party to next
	bool set_head_party_to_next();

	bool has_attr(const char *);
	void set_attr(const char *, int);
	int get_attr(const char *);

	// Battle state variables
	int id;
	int team;
	Strategy strategy{STRATEGY_ATTACKER_NO_DODGE};

private:
	static int instance_count;

	int m_parties_count{0};
	Party m_parties[MAX_NUM_PARTIES];
	Party *m_party_head{nullptr};

	double m_attack_multiplier{1};
	int m_clone_multiplier{1};

	int *search_int_member(const char *);
};

} // namespace GoBattleSim

#endif