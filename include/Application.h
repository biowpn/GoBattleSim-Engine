
#ifndef _APPLICATION_H_
#define _APPLICATION_H_

#include "GoBattleSim.h"

#include <array>
#include <vector>

namespace GoBattleSim
{

enum class BattleMode
{
    PvE,
    PvP,
    BattleMatrix,
};

enum class AggregationMode
{
    None,
    Average,
    Branching
};

struct PvESimInput
{
    std::vector<Player> players;
    int weather;
    int time_limit;
    int num_sims;
    AggregationMode aggregation;
    bool enable_log;
};

struct PvEAverageBattleOutcome
{
    double duration{0};
    double win{0};
    double tdo{0};
    double tdo_percent{0};
    double num_deaths{0};

    // TODO: by-player stats
};

struct PvPSimInput
{
    std::array<PvPPokemon, 2> pokemon;
    std::array<int, 2> num_shields;
    std::array<PvPStrategy, 2> strateies;
    int turn_limit;
    int num_sims;
    AggregationMode aggregation;
    bool enable_log;
};

struct BattleMatrixSimInput
{
    std::vector<PvPPokemon> row_pokemon;
    std::vector<PvPPokemon> col_pokemon;
    bool averge_by_shield;
};

class GoBattleSimApp
{
public:
    static GoBattleSimApp &get();

    /**
     * initialize new simulation. This will clear all output.
     */
    void prepare(const PvESimInput &);

    void prepare(const PvPSimInput &);

    void prepare(const BattleMatrixSimInput &);

    /**
     * run the simulation.
     */
    void run();

    /**
     * collect the latest output.
     */
    void collect(std::vector<PvEBattleOutcome> &); // all
    void collect(PvEAverageBattleOutcome &);       // average

    void collect(std::vector<SimplePvPBattleOutcome> &); // all
    void collect(SimplePvPBattleOutcome &);// average

    void collect(Matrix_t &);

    /**
     * control which type of battle to run.
     */
    BattleMode battle_mode;

    /**
     * control type of aggregation
     */
    AggregationMode aggregation_mode;

private:
    static GoBattleSimApp instance;

    int m_num_sims{0};

    Battle m_pve_battle;
    std::vector<PvEBattleOutcome> m_pve_output;

    SimplePvPBattle m_pvp_battle;
    std::vector<SimplePvPBattleOutcome> m_pvp_output;

    BattleMatrix m_battle_matrix;
    BattleMatrixSimInput m_matrix_input;
};
} // namespace GoBattleSim

#endif
