
/**
 * Convert GoBattleSim structs to/from json. 
 */

#include "GoBattleSim.h"
#include "Application.h"
#include "name_mapping.hpp"

#include "json.hpp"

#include <string>
#include <stdio.h>
#include <vector>

namespace GoBattleSim
{
using nlohmann::json;

typedef std::unordered_map<std::string, std::unordered_map<std::string, double>> Effectiveness_Matrix_t;
typedef std::unordered_map<std::string, std::vector<std::string>> WeatherBoost_Map_t;

template <class T>
T try_get(const json &j, const std::string &key, const T &t_default)
{
    if (j.find(key) != j.end())
    {
        return j.at(key).get<T>();
    }
    else
    {
        return t_default;
    }
}

template <class T>
bool try_get_to(const json &j, const std::string &key, const T &t_default, T &dst)
{
    if (j.find(key) != j.end())
    {
        j.at(key).get_to(dst);
        return true;
    }
    else
    {
        dst = t_default;
        return false;
    }
}

template <class T>
bool try_get_to(const json &j, const std::string &key, T &dst)
{
    return try_get_to(j, key, dst, dst);
}

void to_json(json &j, const MoveEffect &effect)
{
    j["activation_chance"] = effect.activation_chance;
    j["self_attack_stage_delta"] = effect.self_attack_stage_delta;
    j["self_defense_stage_delta"] = effect.self_defense_stage_delta;
    j["target_attack_stage_delta"] = effect.target_attack_stage_delta;
    j["target_defense_stage_delta"] = effect.target_defense_stage_delta;
}

void from_json(const json &j, MoveEffect &effect)
{
    effect.activation_chance = j["activation_chance"];
    effect.self_attack_stage_delta = j["self_attack_stage_delta"];
    effect.self_defense_stage_delta = j["self_defense_stage_delta"];
    effect.target_attack_stage_delta = j["target_attack_stage_delta"];
    effect.target_defense_stage_delta = j["target_defense_stage_delta"];
}

void to_json(json &j, const Move &move)
{
    const auto &typemap = PokeTypeMapping::get();

    j["pokeType"] = typemap.to_name(move.poketype);
    j["power"] = move.power;
    j["energy"] = move.energy;
    j["duration"] = move.duration;
    j["dws"] = move.dws;
    j["effect"] = move.effect;
}

void from_json(const json &j, Move &move)
{
    const auto &typemap = PokeTypeMapping::get();

    move.poketype = typemap.to_idx(j.at("pokeType").get<std::string>());
    j.at("power").get_to(move.power);
    j.at("energy").get_to(move.energy);
    try_get_to(j, "duration", move.duration);
    try_get_to(j, "dws", move.dws);
    try_get_to(j, "effect", move.effect);
}

void to_json(json &j, const Pokemon &pkm)
{
    const auto &typemap = PokeTypeMapping::get();
    j["pokeType1"] = typemap.to_name(pkm.poketype1);
    j["pokeType2"] = typemap.to_name(pkm.poketype2);
    j["attack"] = pkm.attack;
    j["defense"] = pkm.defense;
    j["maxHP"] = pkm.max_hp;
    j["immortal"] = pkm.immortal;
    j["fmove"] = pkm.fmove;
    j["cmoves"] = std::vector<Move>(pkm.cmoves, pkm.cmoves + pkm.cmoves_count);
    j["attack_multiplier"] = pkm.attack_multiplier;
    j["clone_multiplier"] = pkm.clone_multiplier;
    if (pkm.strategy != nullptr)
    {
        j["strategy"] = pkm.strategy->name;
    }
}

void from_json(const json &j, Pokemon &pkm)
{
    const auto &typemap = PokeTypeMapping::get();
    pkm.poketype1 = typemap.to_idx(j.at("pokeType1").get<std::string>());
    pkm.poketype2 = typemap.to_idx(j.at("pokeType2").get<std::string>());

    j.at("attack").get_to(pkm.attack);
    j.at("defense").get_to(pkm.defense);
    j.at("maxHP").get_to(pkm.max_hp);

    j.at("fmove").get_to(pkm.fmove);
    for (const auto &cmove_j : j.at("cmoves"))
    {
        auto cmove = cmove_j.get<Move>();
        pkm.add_cmove(&cmove);
    }

    try_get_to(j, "immortal", pkm.immortal);
    try_get_to(j, "attack_multiplier", pkm.attack_multiplier);
    try_get_to(j, "clone_multiplier", pkm.clone_multiplier);

    std::string strategy_name;
    try_get_to(j, "strategy", strategy_name);
    if (strategy_name.size() > 0)
    {
        for (unsigned i = 0; i < NUM_STRATEGIES; ++i)
        {
            if (BUILT_IN_STRATEGIES[i].name == strategy_name)
            {
                pkm.strategy = &BUILT_IN_STRATEGIES[i];
            }
        }
        if (pkm.strategy == nullptr)
        {
            sprintf(err_msg, "unknown strategy: %s", strategy_name.c_str());
            throw std::runtime_error(err_msg);
        }
    }
}

void to_json(json &j, const PvPPokemon &pkm)
{
    to_json(j, static_cast<const Pokemon &>(pkm));
}

void from_json(const json &j, PvPPokemon &pkm)
{
    from_json(j, static_cast<Pokemon &>(pkm));
}

void to_json(json &j, const Party &party)
{
    std::vector<Pokemon> pkm;
    for (unsigned i = 0; i < party.get_pokemon_count(); ++i)
    {
        pkm.push_back(*party.get_pokemon(i));
    }
    j["pokemon"] = pkm;
    j["revive"] = party.revive_policy();
}

void from_json(const json &j, Party &party)
{
    party.erase_pokemon();
    for (const auto &pkm_j : j.at("pokemon"))
    {
        auto pkm = pkm_j.get<Pokemon>();
        unsigned num_copies = 1;
        try_get_to(pkm_j, "copies", num_copies);
        for (unsigned i = 0; i < num_copies; ++i)
        {
            party.add(&pkm);
        }
    }
    party.revive_policy(try_get(j, "revive", false));
}

void to_json(json &j, const Player &player)
{
    std::vector<Party> parties;
    for (unsigned i = 0; i < player.get_parties_count(); ++i)
    {
        parties.push_back(*player.get_party(i));
    }
    j["parties"] = parties;
    j["team"] = player.team;
    j["strategy"] = player.strategy.name;
    j["attack_multiplier"] = player.get_attack_multiplier();
    j["clone_multiplier"] = player.get_clone_multiplier();
}

void from_json(const json &j, Player &player)
{
    player.erase_parties();
    for (const auto &party_j : j["parties"])
    {
        auto party = party_j.get<Party>();
        player.add(&party);
    }
    player.team = j.at("team");

    if (j.find("attack_multiplier") != j.end())
    {
        player.set_attack_multiplier(j.at("attack_multiplier"));
    }
    if (j.find("clone_multiplier") != j.end())
    {
        player.set_clone_multiplier(j.at("clone_multiplier"));
    }
}

void to_json(json &j, const GameMaster &gm)
{
    const auto &typemap = PokeTypeMapping::get();
    const auto &weathermap = WeatherMapping::get();

    // Type Effectiveness
    Effectiveness_Matrix_t effectiveness;
    for (unsigned i = 0; i < gm.num_types(); ++i)
    {
        std::unordered_map<std::string, double> sub_eff;
        for (unsigned j = 0; j < gm.num_types(); ++j)
        {
            sub_eff[typemap.to_name(j)] = gm.effectiveness(i, j);
        }
        effectiveness[typemap.to_name(i)] = sub_eff;
    }
    j["TypeEffectiveness"] = effectiveness;

    // Weather Settings
    WeatherBoost_Map_t weathers;
    for (unsigned i = 0; i < gm.num_types(); ++i)
    {
        auto pkm_type = typemap.to_name(i);
        auto weather = weathermap.to_name(gm.boosted_weather(i));
        weathers.emplace(weather, std::vector<std::string>());
        weathers[weather].push_back(pkm_type);
    }
    j["WeatherSettings"] = weathers;

    // PvE Battle Settings
    j["PvEBattleSettings"] = {};
    j["PvEBattleSettings"]["sameTypeAttackBonusMultiplier"] = gm.stab_multiplier;
    j["PvEBattleSettings"]["maxEnergy"] = gm.max_energy;
    j["PvEBattleSettings"]["energyDeltaPerHealthLost"] = gm.energy_delta_per_health_lost;
    j["PvEBattleSettings"]["dodgeDurationMs"] = gm.dodge_duration;
    j["PvEBattleSettings"]["dodgeDamageReductionPercent"] = gm.dodge_damage_reduction_percent;
    j["PvEBattleSettings"]["swapDurationMs"] = gm.swap_duration;
    j["PvEBattleSettings"]["weatherAttackBonusMultiplier"] = gm.wab_multiplier;

    j["PvEBattleSettings"]["dodgeWindowMs"] = gm.dodge_window;
    j["PvEBattleSettings"]["rejoinDurationMs"] = gm.rejoin_duration;
    j["PvEBattleSettings"]["itemMenuAnimationTimeMs"] = gm.item_menu_time;
    j["PvEBattleSettings"]["maxReviveTimePerPokemonMs"] = gm.pokemon_revive_time;
    j["PvEBattleSettings"]["fastMoveLagMs"] = gm.fast_attack_lag;
    j["PvEBattleSettings"]["chargedMoveLagMs"] = gm.charged_attack_lag;

    // PvP Battle Settings
    j["PvPBattleSettings"] = {};
    j["PvPBattleSettings"]["sameTypeAttackBonusMultiplier"] = gm.stab_multiplier;
    j["PvPBattleSettings"]["fastAttackBonusMultiplier"] = gm.fast_attack_bonus_multiplier;
    j["PvPBattleSettings"]["chargeAttackBonusMultiplier"] = gm.charged_attack_bonus_multiplier;
    j["PvPBattleSettings"]["maxEnergy"] = gm.max_energy;

    j["PvPBattleSettings"]["quickSwapCooldownDurationSeconds"] = gm.switching_cooldown / 1000;
    j["PvPBattleSettings"]["minimumStatStage"] = gm.min_stage;
    j["PvPBattleSettings"]["maximumStatStage"] = gm.max_stage;

    std::vector<double> atk_stage_multipliers, def_stage_multipliers;
    for (auto s = gm.min_stage; s <= gm.max_stage; ++s)
    {
        atk_stage_multipliers.push_back(gm.atk_stage_multiplier(s));
        def_stage_multipliers.push_back(gm.def_stage_multiplier(s));
    }
    j["PvPBattleSettings"]["attackBuffMultiplier"] = atk_stage_multipliers;
    j["PvPBattleSettings"]["defenseBuffMultiplier"] = def_stage_multipliers;

    // List out supported strategies
    std::vector<std::string> supported_strategies;
    for (unsigned i = 0; i < NUM_STRATEGIES; ++i)
    {
        supported_strategies.push_back(BUILT_IN_STRATEGIES[i].name);
    }
    j["PvPStrategies"] = supported_strategies;
}

void from_json(const json &j, GameMaster &gm)
{
    auto &typemap = PokeTypeMapping::get();
    auto &weathermap = WeatherMapping::get();

    // Type Effectiveness
    auto effectiveness = j.at("TypeEffectiveness").get<Effectiveness_Matrix_t>();

    // set type map first
    typemap.reset();
    gm.num_types(effectiveness.size());
    {
        unsigned i = 0;
        for (const auto &kv : effectiveness)
        {
            typemap.map(i, kv.first);
            ++i;
        }
    }

    // then set effectiveness
    for (const auto &kv : effectiveness)
    {
        auto atk_type = kv.first;
        auto atk_type_i = typemap.to_idx(atk_type);
        for (const auto &kv2 : kv.second)
        {
            auto def_type = kv2.first;
            auto def_type_i = typemap.to_idx(def_type);
            gm.effectiveness(atk_type_i, def_type_i, kv2.second);
        }
    }

    // Weather Settings
    auto weathers = j.at("WeatherSettings").get<WeatherBoost_Map_t>();
    weathermap.reset();
    unsigned weather_i = 0;
    for (const auto &kv : weathers)
    {
        weathermap.map(weather_i, kv.first);
        for (const auto &pkm_type_name : kv.second)
        {
            auto pkm_type_i = typemap.to_idx(pkm_type_name);
            gm.boosted_weather(pkm_type_i, weather_i);
        }
        ++weather_i;
    }

    // PvE Battle Settings
    {
        auto j_pve = j["PvEBattleSettings"];
        try_get_to(j_pve, "sameTypeAttackBonusMultiplier", gm.stab_multiplier);
        try_get_to(j_pve, "maximumEnergy", gm.max_energy);
        try_get_to(j_pve, "energyDeltaPerHealthLost", gm.energy_delta_per_health_lost);
        try_get_to(j_pve, "dodgeDurationMs", gm.dodge_duration);
        try_get_to(j_pve, "dodgeDamageReductionPercent", gm.dodge_damage_reduction_percent);
        try_get_to(j_pve, "swapDurationMs", gm.swap_duration);
        try_get_to(j_pve, "weatherAttackBonusMultiplier", gm.wab_multiplier);
        try_get_to(j_pve, "dodgeWindowMs", gm.dodge_window);
        try_get_to(j_pve, "rejoinDurationMs", gm.rejoin_duration);
        try_get_to(j_pve, "itemMenuAnimationTimeMs", gm.item_menu_time);
        try_get_to(j_pve, "maxReviveTimePerPokemonMs", gm.pokemon_revive_time);
        try_get_to(j_pve, "fastMoveLagMs", gm.fast_attack_lag);
        try_get_to(j_pve, "chargedMoveLagMs", gm.charged_attack_lag);
    }

    // PvP Battle Settings
    {
        auto j_pvp = j["PvPBattleSettings"];

        try_get_to(j_pvp, "sameTypeAttackBonusMultiplier", gm.stab_multiplier);
        try_get_to(j_pvp, "maxEnergy", gm.max_energy);
        try_get_to(j_pvp, "fastAttackBonusMultiplier", gm.fast_attack_bonus_multiplier);
        try_get_to(j_pvp, "chargeAttackBonusMultiplier", gm.charged_attack_bonus_multiplier);

        double swap_duration_seconds = gm.switching_cooldown / 1000;
        try_get_to(j_pvp, "quickSwapCooldownDurationSeconds", swap_duration_seconds);
        gm.switching_cooldown = swap_duration_seconds * 1000;

        try_get_to(j_pvp, "minimumStatStage", gm.min_stage);
        try_get_to(j_pvp, "maximumStatStage", gm.max_stage);
        gm.set_stage_bounds(gm.min_stage, gm.max_stage);
        auto num_stages = gm.max_stage - gm.min_stage + 1;
        std::vector<double> atk_stage_multipliers(num_stages), def_stage_multipliers(num_stages);
        try_get_to(j_pvp, "attackBuffMultiplier", atk_stage_multipliers);
        try_get_to(j_pvp, "defenseBuffMultiplier", def_stage_multipliers);
        for (auto s = gm.min_stage; s <= gm.max_stage; ++s)
        {
            gm.atk_stage_multiplier(s, atk_stage_multipliers.at(s - gm.min_stage));
            gm.def_stage_multiplier(s, def_stage_multipliers.at(s - gm.min_stage));
        }
    }
}

void to_json(json &j, const ActionType &action_type)
{
    switch (action_type)
    {
    case ActionType::None:
        j = "None";
        break;
    case ActionType::Wait:
        j = "Wait";
        break;
    case ActionType::Fast:
        j = "Fast";
        break;
    case ActionType::Charged:
        j = "Charged";
        break;
    case ActionType::Dodge:
        j = "Dodge";
        break;
    case ActionType::Switch:
        j = "Switch";
        break;
    default:
        j = "Unknown";
        break;
    }
}

void to_json(json &j, const Action &action)
{
    j["time"] = action.time;
    j["type"] = action.type;
    j["delay"] = action.delay;
    j["value"] = action.value;
}

void to_json(json &j, const EventType &event_type)
{
    switch (event_type)
    {
    case EventType::None:
        j = "None";
        break;
    case EventType::Announce:
        j = "Announce";
        break;
    case EventType::Free:
        j = "Free";
        break;
    case EventType::Fast:
        j = "Fast";
        break;
    case EventType::Charged:
        j = "Charged";
        break;
    case EventType::Damage:
        j = "Damage";
        break;
    case EventType::Dodge:
        j = "Dodge";
        break;
    case EventType::Enter:
        j = "Enter";
        break;
    default:
        j = "Unknown";
        break;
    }
}

void to_json(json &j, const TimelineEvent &event)
{
    j["time"] = event.time;
    j["type"] = event.type;
    j["player"] = event.player;
    j["value"] = event.value;
}

void from_json(const json &j, BattleMode &mode)
{
    auto mode_str = j.get<std::string>();
    std::for_each(mode_str.begin(), mode_str.end(), ::tolower);
    if (mode_str == "pve" || mode_str == "raid" || mode_str == "gym")
    {
        mode = BattleMode::PvE;
    }
    else if (mode_str == "pvp")
    {
        mode = BattleMode::PvP;
    }
    else if (mode_str == "battlematrix")
    {
        mode = BattleMode::BattleMatrix;
    }
    else
    {
        sprintf(err_msg, "unknown battle mode '%s'", mode_str.c_str());
        throw std::runtime_error(err_msg);
    }
}

void from_json(const json &j, PvESimInput &input)
{
    const auto &weathermap = WeatherMapping::get();

    j.at("players").get_to(input.players);
    j.at("timelimit").get_to(input.time_limit);

    std::string weather_name{""};
    try_get_to(j, "weather", weather_name);
    input.weather = weathermap.to_idx(weather_name);

    try_get_to(j, "numSims", 1u, input.num_sims);
    try_get_to(j, "enableLog", false, input.enable_log);

    input.aggregation = AggregationMode::None;
    if (j.find("aggregation") != j.end())
    {
        auto agg_str = j.at("aggregation").get<std::string>();
        std::for_each(agg_str.begin(), agg_str.end(), ::tolower);
        if (agg_str == "none" || agg_str == "enum")
        {
            input.aggregation = AggregationMode::None;
        }
        else if (agg_str == "average" || agg_str == "avrg")
        {
            input.aggregation = AggregationMode::Average;
        }
        else if (agg_str == "branching" || agg_str == "tree")
        {
            input.aggregation = AggregationMode::Branching;
        }
        else
        {
            sprintf(err_msg, "unknown aggregation ('%s')", agg_str.c_str());
            throw std::runtime_error(err_msg);
        }
    }
}

void to_json(json &j, const PokemonState &pkm_st)
{
    j["hp"] = pkm_st.hp;
    j["maxHP"] = pkm_st.max_hp;
    j["energy"] = pkm_st.energy;
    j["tdo"] = pkm_st.tdo;
    j["tdoFast"] = pkm_st.tdo_fast;
    j["numDeaths"] = pkm_st.num_deaths;
    j["duration"] = pkm_st.duration / 1000.0;
    j["dps"] = pkm_st.tdo / (pkm_st.duration / 1000.0);
    j["numFastAttacks"] = pkm_st.num_fmoves_used;
    j["numChargedAttacks"] = pkm_st.num_cmoves_used;
}

void to_json(json &j, const PvEBattleOutcome &outcome)
{
    j["statistics"] = {};
    j["statistics"]["duration"] = outcome.duration / 1000.0;
    j["statistics"]["win"] = outcome.win ? 1 : 0;
    j["statistics"]["tdo"] = outcome.tdo;
    j["statistics"]["tdoPercent"] = outcome.tdo_percent * 100;
    j["statistics"]["dps"] = outcome.tdo / (outcome.duration / 1000.0);
    j["statistics"]["numDeaths"] = outcome.num_deaths;
    j["pokemon"] = outcome.pokemon_stats;
    j["battleLog"] = outcome.battle_log;
}

void to_json(json &j, const AveragePokemonState &pkm_st)
{
    j["hp"] = pkm_st.hp;
    j["maxHP"] = pkm_st.max_hp;
    j["energy"] = pkm_st.energy;
    j["tdo"] = pkm_st.tdo;
    j["tdoFast"] = pkm_st.tdo_fast;
    j["numDeaths"] = pkm_st.num_deaths;
    j["duration"] = pkm_st.duration / 1000.0;
    j["dps"] = pkm_st.tdo / (pkm_st.duration / 1000.0);
    j["numFastAttacks"] = pkm_st.num_fmoves_used;
    j["numChargedAttacks"] = pkm_st.num_cmoves_used;
}

void to_json(json &j, const PvEAverageBattleOutcome &outcome)
{
    j["statistics"] = {};
    j["statistics"]["duration"] = outcome.duration / 1000.0;
    j["statistics"]["win"] = outcome.win;
    j["statistics"]["tdo"] = outcome.tdo;
    j["statistics"]["tdoPercent"] = outcome.tdo_percent * 100;
    j["statistics"]["dps"] = outcome.tdo / (outcome.duration / 1000.0);
    j["statistics"]["numDeaths"] = outcome.num_deaths;
    j["pokemon"] = outcome.pokemon_stats;
    j["numSims"] = outcome.num_sims;
}

void from_json(const json &j, PvPSimpleSimInput &input)
{
    j["pokemon"].get_to(input.pokemon);

    try_get_to(j, "numShields", {2, 2}, input.num_shields);
    try_get_to(j, "timelimit", 1800, input.turn_limit);
    try_get_to(j, "numSims", 1, input.num_sims);
    try_get_to(j, "aggregation", AggregationMode::Branching, input.aggregation);
    try_get_to(j, "enableLog", false, input.enable_log);
}

void to_json(json &j, const SimplePvPBattleOutcome &output)
{
    j["statistics"] = {};
    j["statistics"]["tdoPercent"] = output.tdo_percent;
    j["statistics"]["duration"] = output.duration;
    j["battleLog"] = output.battle_log;
}

void from_json(const json &j, BattleMatrixSimInput &input)
{
    j["rowPokemon"].get_to(input.row_pokemon);
    j["colPokemon"].get_to(input.col_pokemon);
    if (input.col_pokemon.empty())
    {
        input.col_pokemon = input.row_pokemon;
    }
    if (input.row_pokemon.empty())
    {
        input.row_pokemon = input.col_pokemon;
    }

    try_get_to(j, "avergeByShield", false, input.averge_by_shield);
}

}; // namespace GoBattleSim
