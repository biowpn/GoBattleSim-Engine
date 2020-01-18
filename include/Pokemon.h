
#ifndef _POKEMON_H_
#define _POKEMON_H_

#include "Move.h"

namespace GoBattleSim
{

constexpr unsigned MAX_NUM_CMOVES = 3;

// forward declaration
class Strategy;

class Pokemon
{
public:
	Pokemon() = default;
	Pokemon(int, int, double, double, int);
	Pokemon(const Pokemon &);
	Pokemon &operator=(const Pokemon &);
	~Pokemon();

	const Move *get_fmove(unsigned) const;
	void add_fmove(const Move *);
	const Move *get_cmove(unsigned) const;
	unsigned get_cmoves_count() const;
	void add_cmove(const Move *);
	void erase_cmoves();

	// Members
	int poketype1{0};
	int poketype2{0};
	double attack{0};
	double defense{0};
	int max_hp{0};
	bool immortal{false};

	Move fmove;
	Move cmoves[MAX_NUM_CMOVES];
	Move *cmove{nullptr};
	unsigned cmoves_count{0};

	const Strategy *strategy{nullptr};
};

// Damage calculation as a public function
int calc_damage(const Pokemon *attacker,
				const Move *move,
				const Pokemon *defender,
				double multiplier);

} // namespace GoBattleSim

#endif