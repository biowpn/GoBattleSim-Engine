
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
	Party(const Party &);
	~Party();

	// Interface functions
	Pokemon *get_pokemon(int);
	const Pokemon *get_pokemon(int) const;
	void add(const Pokemon *);
	void erase_pokemon();
	bool has_attr(const char *);
	int get_attr(const char *);
	void set_attr(const char *, int);
	// End of Interface functions

	// Battle functions
	void init();
	void heal();

	int get_pokemon_count() const;

	// get the internal addresses of Pokemon objects, avoid copying
	Pokemon **get_all_pokemon(Pokemon **out_first);

	void update(const Pokemon *);

	Pokemon *get_head();
	bool set_head(const Pokemon *);
	bool set_head(int);
	bool set_head_to_next();
	bool revive();

private:
	int m_revive_policy;
	int m_revive_policy_init;

	Pokemon *m_pokemon_head{nullptr};
	Pokemon m_pokemon[MAX_NUM_POKEMON];
	int m_pokemon_count{0};

	int *search_int_member(const char *);
};

} // namespace GoBattleSim

#endif