
#include "BattleMatrix.h"

#include <thread>

namespace GoBattleSim
{

double get_battle_score(const PvPPokemon *t_pkm_0, const PvPPokemon *t_pkm_1, int t_num_shields_0, int t_num_shields_1)
{
	SimplePvPBattle battle(t_pkm_0, t_pkm_1);
	battle.set_num_shields_max(t_num_shields_0, t_num_shields_1);
	battle.init();
	battle.start();
	SimplePvPBattleOutcome temp_outcome = battle.get_outcome();
	double tdo_0 = temp_outcome.tdo_percent[0], tdo_1 = temp_outcome.tdo_percent[1];
	return (tdo_0 < 1 ? tdo_0 : 1) - (tdo_1 < 1 ? tdo_1 : 1);
}

BattleMatrix::BattleMatrix(const PvPPokemon **t_row_pkm, int t_row_size, const PvPPokemon **t_col_pkm, int t_col_size, bool t_enum_shields)
{
	m_row_size = t_row_size;
	m_row_pkm = new PvPPokemon *[m_row_size];
	for (int i = 0; i < m_row_size; ++i)
	{
		m_row_pkm[i] = new PvPPokemon(*t_row_pkm[i]);
	}

	m_col_size = t_col_size;
	m_col_pkm = new PvPPokemon *[t_col_size];
	for (int i = 0; i < t_col_size; ++i)
	{
		m_col_pkm[i] = new PvPPokemon(*t_col_pkm[i]);
	}

	m_matrix = new double *[m_row_size];
	for (int i = 0; i < m_row_size; ++i)
	{
		m_matrix[i] = new double[m_col_size];
	}

	m_enum_shields = t_enum_shields;
}

BattleMatrix::~BattleMatrix()
{
	for (int i = 0; i < m_row_size; ++i)
	{
		delete m_row_pkm[i];
		delete[] m_matrix[i];
	}
	for (int i = 0; i < m_col_size; ++i)
	{
		delete m_col_pkm[i];
	}
	delete[] m_row_pkm;
	delete[] m_col_pkm;
	delete[] m_matrix;
}

void worker(double **matrix,
			PvPPokemon **row_pkm,
			PvPPokemon **col_pkm,
			int row_first,
			int row_last,
			int col_first,
			int col_last,
			bool enum_shields)
{
	if (enum_shields)
	{
		for (int i = row_first; i < row_last; ++i)
		{
			for (int j = col_first; j < col_last; ++j)
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
		for (int i = row_first; i < row_last; ++i)
		{
			for (int j = col_first; j < col_last; ++j)
			{
				matrix[i][j] = get_battle_score(row_pkm[i], col_pkm[j], row_pkm[i]->num_shields_max, col_pkm[j]->num_shields_max);
			}
		}
	}
}

void BattleMatrix::run()
{
	int cpu_count = std::thread::hardware_concurrency();
	std::thread *threads = new std::thread[cpu_count];

	// Round-Robin distribute jobs
	int total_job_count = 0;
	if (m_row_size >= m_col_size)
	{
		// Divide in row
		for (int i = 0; i < cpu_count; ++i)
		{
			int cur_job_count = (m_row_size / cpu_count) + ((m_row_size % cpu_count > i) ? 1 : 0);
			threads[i] = std::thread(worker, m_matrix, m_row_pkm, m_col_pkm, total_job_count, total_job_count + cur_job_count, 0, m_col_size, m_enum_shields);
			total_job_count += cur_job_count;
		}
	}
	else
	{
		// Divide in column
		for (int i = 0; i < cpu_count; ++i)
		{
			int cur_job_count = (m_col_size / cpu_count) + ((m_col_size % cpu_count > i) ? 1 : 0);
			threads[i] = std::thread(worker, m_matrix, m_row_pkm, m_col_pkm, 0, m_row_size, total_job_count, total_job_count + cur_job_count, m_enum_shields);
			total_job_count += cur_job_count;
		}
	}

	for (int i = 0; i < cpu_count; ++i)
	{
		threads[i].join();
	}
	delete[] threads;
}

void BattleMatrix::get(double **r_matrix)
{
	for (int i = 0; i < m_row_size; ++i)
	{
		for (int j = 0; j < m_col_size; ++j)
		{
			r_matrix[i][j] = m_matrix[i][j];
		}
	}
}

void BattleMatrix::get(double *r_matrix)
{
	for (int i = 0; i < m_row_size; ++i)
	{
		for (int j = 0; j < m_col_size; ++j)
		{
			r_matrix[i * m_col_size + j] = m_matrix[i][j];
		}
	}
}

} // namespace GoBattleSim
