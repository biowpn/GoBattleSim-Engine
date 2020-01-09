
#include "BattleMatrix.h"

/**
 * Emscripten does not support multi-threading well,
 * or at least I couldn't get it working with the compile flags:
 * 
 * 		-s ALLOW_MEMORY_GROWTH=1 -s USE_PTHREADS=1 -s PTHREAD_POOL_SIZE=2
 * 
 * The compiler will emit strange error, duh.
 * 
 * When compiled with Emscripten, fallback to no multi-threading
 */
#ifndef __EMSCRIPTEN__
#include <thread>
#endif

namespace GoBattleSim
{

double get_battle_score(const PvPPokemon &t_pkm_0, const PvPPokemon &t_pkm_1, int t_num_shields_0, int t_num_shields_1)
{
	SimplePvPBattle battle;
	battle.set_pokemon(t_pkm_0, t_pkm_1);
	battle.set_num_shields_max(t_num_shields_0, t_num_shields_1);
	battle.init();
	battle.start();
	SimplePvPBattleOutcome temp_outcome = battle.get_outcome();
	double tdo_0 = temp_outcome.tdo_percent[0], tdo_1 = temp_outcome.tdo_percent[1];
	return (tdo_0 < 1 ? tdo_0 : 1) - (tdo_1 < 1 ? tdo_1 : 1);
}

void BattleMatrix::set(const std::vector<PvPPokemon> &row_pokemon,
					   const std::vector<PvPPokemon> &col_pokemon,
					   bool average_by_shield)
{
	m_row_pkm = row_pokemon;
	m_col_pkm = col_pokemon;
	m_average_by_shield = average_by_shield;

	m_matrix.clear();
	for (size_t i = 0; i < row_pokemon.size(); ++i)
	{
		m_matrix.push_back(std::vector<double>(m_col_pkm.size()));
	}
}

void worker(Matrix_t &matrix,
			const std::vector<PvPPokemon> &row_pkm,
			const std::vector<PvPPokemon> &col_pkm,
			unsigned row_first,
			unsigned row_last,
			unsigned col_first,
			unsigned col_last,
			bool averge_by_shield)
{
	if (averge_by_shield)
	{
		for (unsigned i = row_first; i < row_last; ++i)
		{
			for (unsigned j = col_first; j < col_last; ++j)
			{
				double score_0_0 = get_battle_score(row_pkm[i], col_pkm[j], 0, 0);
				double score_0_1 = get_battle_score(row_pkm[i], col_pkm[j], 0, 1);
				double score_1_1 = get_battle_score(row_pkm[i], col_pkm[j], 1, 0);
				double score_1_0 = get_battle_score(row_pkm[i], col_pkm[j], 1, 1);
				double score_2_2 = get_battle_score(row_pkm[i], col_pkm[j], 2, 2);
				matrix[i][j] = (score_0_0 + score_0_1 + score_1_1 + score_1_0 + score_2_2) / 5;
			}
		}
	}
	else
	{
		for (unsigned i = row_first; i < row_last; ++i)
		{
			for (unsigned j = col_first; j < col_last; ++j)
			{
				matrix[i][j] = get_battle_score(row_pkm[i], col_pkm[j], row_pkm[i].num_shields_max, col_pkm[j].num_shields_max);
			}
		}
	}
}

#ifndef __EMSCRIPTEN__

void BattleMatrix::run()
{
	auto cpu_count = std::thread::hardware_concurrency();
	std::vector<std::thread> threads(cpu_count);

	// Round-Robin distribute jobs
	unsigned total_job_count = 0;
	unsigned row_size = m_row_pkm.size(), col_size = m_col_pkm.size();
	if (row_size >= col_size)
	{
		// Divide in row
		for (unsigned i = 0; i < cpu_count; ++i)
		{
			unsigned cur_job_count = (row_size / cpu_count) + ((row_size % cpu_count > i) ? 1 : 0);
			threads[i] = std::thread(worker,
									 std::ref(m_matrix),
									 std::ref(m_row_pkm),
									 std::ref(m_col_pkm),
									 total_job_count,
									 total_job_count + cur_job_count,
									 (unsigned)0,
									 col_size,
									 m_average_by_shield);
			total_job_count += cur_job_count;
		}
	}
	else
	{
		// Divide in column
		for (unsigned i = 0; i < cpu_count; ++i)
		{
			unsigned cur_job_count = (row_size / cpu_count) + ((col_size % cpu_count > i) ? 1 : 0);
			threads[i] = std::thread(worker,
									 std::ref(m_matrix),
									 std::ref(m_row_pkm),
									 std::ref(m_col_pkm),
									 (unsigned)0,
									 row_size,
									 total_job_count,
									 total_job_count + cur_job_count,
									 m_average_by_shield);
			total_job_count += cur_job_count;
		}
	}

	for (unsigned i = 0; i < cpu_count; ++i)
	{
		threads[i].join();
	}
}

#else

void BattleMatrix::run()
{
	worker(m_matrix,
		   m_row_pkm,
		   m_col_pkm,
		   0,
		   m_row_pkm.size(),
		   0,
		   m_col_pkm.size(),
		   m_average_by_shield);
}

#endif

const Matrix_t &BattleMatrix::get() const
{
	return m_matrix;
}

} // namespace GoBattleSim
