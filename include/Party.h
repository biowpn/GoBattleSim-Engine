
#ifndef _PARTY_H_
#define _PARTY_H_

#include "Pokemon.h"

namespace GoBattleSim
{

constexpr unsigned MAX_NUM_POKEMON = 6;

class Party
{
public:
	Party() = default;
	Party &operator=(const Party &);
	Party(const Party &);
	~Party();

	Pokemon *get_pokemon(unsigned);
	const Pokemon *get_pokemon(unsigned) const;
	unsigned get_pokemon_count() const;

	// get the internal addresses of Pokemon objects, avoid copying
	Pokemon **get_all_pokemon(Pokemon **out_first);
	void add(const Pokemon *);
	void erase_pokemon();

	Pokemon *get_head();
	bool set_head(const Pokemon *);

	void init();

	// whether to revive or not
	bool revive_policy{false}; 
	// delay before entering battle
	unsigned enter_delay{0};

private:
	Pokemon *m_pokemon_head{nullptr};
	Pokemon m_pokemon[MAX_NUM_POKEMON];
	unsigned m_pokemon_count{0};
};

} // namespace GoBattleSim

#endif