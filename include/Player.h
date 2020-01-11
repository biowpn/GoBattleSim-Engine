
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
	Player() = default;
	Player(const Player &);
	Player &operator=(const Player &);
	~Player();

	Party *get_party(unsigned);
	const Party *get_party(unsigned) const;
	unsigned get_parties_count() const;
	void add(const Party *);
	void erase_parties();
	void set_strategy(const Strategy &);

	Party *get_head_party();
	unsigned get_pokemon_count() const;
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

	// Battle state variables
	int team{0};
	double attack_multiplier{1.0};
	int clone_multiplier{1};
	Strategy strategy{STRATEGY_ATTACKER_NO_DODGE};

private:
	unsigned m_parties_count{0};
	Party m_parties[MAX_NUM_PARTIES];
	Party *m_party_head{nullptr};
};

} // namespace GoBattleSim

#endif