
#ifndef _PARTY_H_
#define _PARTY_H_

#include "Pokemon.h"

namespace GoBattleSim
{

constexpr unsigned MAX_NUM_POKEMON = 6;

class Party
{
public:
	Party();
	Party &operator=(const Party &);
	Party(const Party &);
	~Party();

	Pokemon *get_pokemon(int);
	const Pokemon *get_pokemon(int) const;
	unsigned get_pokemon_count() const;

	// get the internal addresses of Pokemon objects, avoid copying
	Pokemon **get_all_pokemon(Pokemon **out_first);
	void add(const Pokemon *);
	void update(const Pokemon *);
	void erase_pokemon();

	Pokemon *get_head();
	bool set_head(const Pokemon *);
	bool set_head(int);

	void init();

	// number of times to revive
	void set_revive_policy(int);
	int get_revive_policy() const;

	// this function only asks the party whether to revive (since it depends on the revive policy).
	// the actual reviving is done in Battle::revive
	bool revive();

private:
	int m_revive_quota;
	int m_revive_policy;

	Pokemon *m_pokemon_head{nullptr};
	Pokemon m_pokemon[MAX_NUM_POKEMON];
	unsigned m_pokemon_count{0};
};

} // namespace GoBattleSim

#endif