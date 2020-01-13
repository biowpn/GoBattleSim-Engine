
#include "Application.h"

#include <stdexcept>
#include <stdio.h>

namespace GoBattleSim
{

GoBattleSimApp GoBattleSimApp::instance;

GoBattleSimApp &GoBattleSimApp::get()
{
    return instance;
}

void GoBattleSimApp::prepare(const PvESimInput &input)
{
    battle_mode = BattleMode::PvE;
    aggregation_mode = input.aggregation;
    m_num_sims = input.num_sims;

    if (input.time_limit <= 0)
    {
        sprintf(err_msg, "timelimit must be positive (got %d)", input.time_limit);
        throw std::runtime_error(err_msg);
    }
    m_pve_battle.erase_players();
    for (const auto &player : input.players)
    {
        m_pve_battle.add_player(&player);
    }
    m_pve_battle.set_time_limit(input.time_limit);
    m_pve_battle.set_weather(input.weather);
    m_pve_battle.set_background_dps(input.background_dps);
    m_pve_battle.set_enable_log(input.enable_log);

    m_pve_output.clear();
}

void GoBattleSimApp::prepare(const PvPSimpleSimInput &input)
{
    battle_mode = BattleMode::PvP;
    aggregation_mode = input.aggregation;
    m_num_sims = input.num_sims;

    m_pvp_battle.set_pokemon(input.pokemon[0], input.pokemon[1]);
    m_pvp_battle.set_strategy(input.strateies[0], input.strateies[1]);
    m_pvp_battle.set_num_shields_max(input.num_shields[0], input.num_shields[1]);
    if (aggregation_mode == AggregationMode::Branching)
    {
        m_pvp_battle.set_enable_branching(true);
    }
    m_pvp_battle.set_enable_log(input.enable_log);

    m_pvp_output.clear();
}

void GoBattleSimApp::prepare(const BattleMatrixSimInput &input)
{
    battle_mode = BattleMode::BattleMatrix;
    m_battle_matrix.set(input.row_pokemon, input.col_pokemon, input.averge_by_shield);
}

void GoBattleSimApp::run()
{
    if (battle_mode == BattleMode::PvE)
    {
        if (aggregation_mode == AggregationMode::Average)
        {
            m_pve_output_avg = {};
            for (unsigned i = 0; i < m_num_sims; ++i)
            {
                m_pve_battle.init();
                m_pve_battle.start();
                auto output = m_pve_battle.get_outcome(1);
                add_to(m_pve_output_avg, output);
            }
            div_by(m_pve_output_avg, m_num_sims);
        }
        else
        {
            for (unsigned i = 0; i < m_num_sims; ++i)
            {
                m_pve_battle.init();
                m_pve_battle.start();
                auto output = m_pve_battle.get_outcome(1);
                m_pve_output.push_back(output);
            }
        }
    }
    else if (battle_mode == BattleMode::PvP)
    {
        for (unsigned i = 0; i < m_num_sims; ++i)
        {
            m_pvp_battle.init();
            m_pvp_battle.start();
            auto output = m_pvp_battle.get_outcome();
            m_pvp_output.push_back(output);
        }
    }
    else if (battle_mode == BattleMode::BattleMatrix)
    {
        m_battle_matrix.run();
    }
    else
    {
        sprintf(err_msg, "unknown battle mode: %d", (int)battle_mode);
        throw std::runtime_error(err_msg);
    }
}

void GoBattleSimApp::collect(std::vector<PvEBattleOutcome> &outputs)
{
    outputs.insert(outputs.end(), m_pve_output.begin(), m_pve_output.end());
}

void GoBattleSimApp::collect(PvEAverageBattleOutcome &output)
{
    output = m_pve_output_avg;
}

void GoBattleSimApp::collect(std::vector<SimplePvPBattleOutcome> &outputs)
{
    outputs.insert(outputs.end(), m_pvp_output.begin(), m_pvp_output.end());
}

void GoBattleSimApp::collect(SimplePvPBattleOutcome &output)
{
    if (aggregation_mode == AggregationMode::Branching)
    {
        output = m_pvp_output.back();
    }
    else
    {
        output.tdo_percent[0] = 0;
        output.tdo_percent[0] = 1;
        for (const auto &sim_output : m_pvp_output)
        {
            output.tdo_percent[0] += sim_output.tdo_percent[0];
            output.tdo_percent[1] += sim_output.tdo_percent[1];
        }
        auto count = m_pvp_output.size();
        output.tdo_percent[0] /= count;
        output.tdo_percent[1] /= count;
    }
}

void GoBattleSimApp::collect(Matrix_t &output)
{
    output = m_battle_matrix.get();
}

void GoBattleSimApp::add_to(PvEAverageBattleOutcome &sum, PvEBattleOutcome cur)
{
    auto pokemon_count = cur.pokemon_stats.size();
    if (sum.pokemon_stats.size() == 0)
    {
        sum.pokemon_stats.resize(pokemon_count);
    }
    if (sum.pokemon_stats.size() != pokemon_count)
    {
        sprintf(err_msg, "mismatch Pokemon count when averaging battle outcomes (expect %zu, got %zu)",
                pokemon_count, sum.pokemon_stats.size());
        throw std::runtime_error(err_msg);
    }

    sum.duration += cur.duration;
    sum.win += cur.win ? 1 : 0;
    sum.tdo += cur.tdo;
    sum.tdo_percent += cur.tdo_percent;
    sum.num_deaths += cur.num_deaths;

    for (size_t i = 0; i < pokemon_count; ++i)
    {
        sum.pokemon_stats[i].max_hp = cur.pokemon_stats[i].max_hp;
        sum.pokemon_stats[i].hp += cur.pokemon_stats[i].hp;
        sum.pokemon_stats[i].energy += cur.pokemon_stats[i].energy;
        sum.pokemon_stats[i].tdo += cur.pokemon_stats[i].tdo;
        sum.pokemon_stats[i].tdo_fast += cur.pokemon_stats[i].tdo_fast;
        sum.pokemon_stats[i].duration += cur.pokemon_stats[i].duration;
        sum.pokemon_stats[i].num_deaths += cur.pokemon_stats[i].num_deaths;
        sum.pokemon_stats[i].num_fmoves_used += cur.pokemon_stats[i].num_fmoves_used;
        sum.pokemon_stats[i].num_cmoves_used += cur.pokemon_stats[i].num_cmoves_used;
    }
}

void GoBattleSimApp::div_by(PvEAverageBattleOutcome &sum, unsigned num_sims)
{
    sum.num_sims = num_sims;
    sum.duration /= sum.num_sims;
    sum.win /= sum.num_sims;
    sum.tdo /= sum.num_sims;
    sum.tdo_percent /= sum.num_sims;
    sum.num_deaths /= sum.num_sims;
    for (size_t i = 0; i < sum.pokemon_stats.size(); ++i)
    {
        sum.pokemon_stats[i].hp /= sum.num_sims;
        sum.pokemon_stats[i].energy /= sum.num_sims;
        sum.pokemon_stats[i].tdo /= sum.num_sims;
        sum.pokemon_stats[i].tdo_fast /= sum.num_sims;
        sum.pokemon_stats[i].duration /= sum.num_sims;
        sum.pokemon_stats[i].num_deaths /= sum.num_sims;
        sum.pokemon_stats[i].num_fmoves_used /= sum.num_sims;
        sum.pokemon_stats[i].num_cmoves_used /= sum.num_sims;
    }
}

} // namespace GoBattleSim
