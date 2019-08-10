
#ifndef _BATTLE_MATRIX_H_
#define _BATTLE_MATRIX_H_

#include <thread>

#include "SimplePvPBattle.h"


double get_battle_score(const PvPPokemon*, const PvPPokemon*, int, int);


class BattleMatrix
{
public:
	BattleMatrix(const PvPPokemon**, int, const PvPPokemon**, int, bool=false);
	~BattleMatrix();
	
	void run();
	void get(double**);

	
protected:
	PvPPokemon** m_row_pkm;
	int m_row_size;
	PvPPokemon** m_col_pkm;
	int m_col_size;
	
	bool m_enum_shields;
	double** m_matrix;
	
};



#endif


