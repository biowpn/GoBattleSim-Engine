
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
    int weather{-1};
    int time_limit{0};
    unsigned num_sims{0};
    AggregationMode aggregation{AggregationMode::None};
    bool enable_log{false};
};

struct AveragePokemonState
{
    int max_hp{0};
    double hp{0.0};
    double energy{0.0};
    double tdo{0.0};
    double tdo_fast{0.0};
    double duration{0.0};
    double num_deaths{0.0};
    double num_fmoves_used{0.0};
    double num_cmoves_used{0.0};
};

struct PvEAverageBattleOutcome
{
    unsigned num_sims{0};
    double duration{0};
    double win{0};
    double tdo{0};
    double tdo_percent{0};
    double num_deaths{0};
    std::vector<AveragePokemonState> pokemon_stats;
};

struct PvPSimInput
{
    std::array<PvPPokemon, 2> pokemon;
    std::array<int, 2> num_shields;
    std::array<PvPStrategy, 2> strateies;
    int turn_limit{0};
    int num_sims{0};
    AggregationMode aggregation;
    bool enable_log{false};
};

struct BattleMatrixSimInput
{
    std::vector<PvPPokemon> row_pokemon;
    std::vector<PvPPokemon> col_pokemon;
    bool averge_by_shield{false};
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
    void collect(SimplePvPBattleOutcome &);              // average

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

    static void add_to(PvEAverageBattleOutcome &, PvEBattleOutcome);
    static void div_by(PvEAverageBattleOutcome &, unsigned);

    unsigned m_num_sims{0};

    Battle m_pve_battle;
    std::vector<PvEBattleOutcome> m_pve_output;
    PvEAverageBattleOutcome m_pve_output_avg;

    SimplePvPBattle m_pvp_battle;
    std::vector<SimplePvPBattleOutcome> m_pvp_output;

    BattleMatrix m_battle_matrix;
    BattleMatrixSimInput m_matrix_input;
};
} // namespace GoBattleSim

#endif
