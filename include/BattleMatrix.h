
#ifndef _BATTLE_MATRIX_H_
#define _BATTLE_MATRIX_H_

#include "SimplePvPBattle.h"

#include <vector>

namespace GoBattleSim
{

typedef std::vector<std::vector<double>> Matrix_t;

double get_battle_score(const PvPPokemon &pkm1,
						const PvPPokemon &pkm2,
						int num_shields_1,
						int num_shields_2);

class BattleMatrix
{
public:
	void set(const std::vector<PvPPokemon> &row_pokemon,
			 const std::vector<PvPPokemon> &col_pokemon,
			 bool average_by_shield);

	void run();

	const Matrix_t &get() const;

protected:
	std::vector<PvPPokemon> m_row_pkm;
	std::vector<PvPPokemon> m_col_pkm;

	bool m_average_by_shield;

	Matrix_t m_matrix;
};

} // namespace GoBattleSim

#endif
