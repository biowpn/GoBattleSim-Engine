
/**
 * Convert GoBattleSim structs to/from json. 
 */

#include "GoBattleSim.h"

#include "json.hpp"

#include <vector>
#include <string>

namespace GoBattleSim
{
using nlohmann::json;

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
    j["poketype"] = move.poketype;
    j["power"] = move.power;
    j["energy"] = move.energy;
    j["duration"] = move.duration;
    j["dws"] = move.dws;
    j["effect"] = move.effect;
}

void from_json(const json &j, Move &move)
{
    move.poketype = j["poketype"];
    move.power = j["power"];
    move.energy = j["energy"];
    move.duration = j["duration"];
    move.dws = j["dws"];
    move.effect = j["effect"];
}

void to_json(json &j, const Pokemon &pkm)
{
    j["id"] = pkm.id;
    j["poketype1"] = pkm.poketype1;
    j["poketype2"] = pkm.poketype2;
    j["attack"] = pkm.attack;
    j["defense"] = pkm.defense;
    j["max_hp"] = pkm.max_hp;
    j["immortal"] = pkm.immortal;
    j["fmove"] = pkm.fmove;
    j["cmoves"] = std::vector<Move>(pkm.cmoves, pkm.cmoves + pkm.cmoves_count);
    j["attack_multiplier"] = pkm.attack_multiplier;
    j["clone_multiplier"] = pkm.clone_multiplier;
}

void from_json(const json &j, Pokemon &pkm)
{
    pkm.id = j["id"];
    pkm.poketype1 = j["poketype1"];
    pkm.poketype2 = j["poketype2"];
    pkm.attack = j["attack"];
    pkm.defense = j["defense"];
    pkm.max_hp = j["max_hp"];
    pkm.immortal = j["immortal"];
    pkm.fmove = j["fmove"];
    for (const auto &cmove_j : j["cmove"])
    {
        auto cmove = cmove_j.get<Move>();
        pkm.add_cmove(&cmove);
    }
    pkm.attack_multiplier = j["attack_multiplier"];
    pkm.clone_multiplier = j["clone_multiplier"];
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
    for (int i = 0; i < party.get_pokemon_count(); ++i)
    {
        pkm.push_back(*party.get_pokemon(i));
    }
    j["revive_policy"] = party.get_revive_policy();
}

void from_json(const json &j, Party &party)
{
    party.erase_pokemon();
    for (const auto &pkm_j : j["pokemon"])
    {
        auto pkm = pkm_j.get<Pokemon>();
        party.add(&pkm);
    }
    party.set_revive_policy(j["revive_policy"]);
}

void to_json(json &j, const Player &player)
{
    std::vector<Party> parties;
    for (int i = 0; i < player.get_parties_count(); ++i)
    {
        parties.push_back(*player.get_party(i));
    }
    j["parties"] = parties;
    j["team"] = player.team;
    j["id"] = player.id;
    j["strategy"] = std::string(player.strategy.name);
}

void from_json(const json &j, Player &player)
{
    player.erase_parties();
    for (const auto &party_j : j["parties"])
    {
        auto party = party_j.get<Party>();
        player.add(&party);
    }
    player.team = j["team"];
    player.id = j["id"];
    auto strategy_name = j["strategy"].get<std::string>();
    for (int i = 0; i < NUM_STRATEGIES; ++i)
    {
        if (strcmp(BUILT_IN_STRATEGIES[i].name, strategy_name.c_str()) == 0)
        {
            player.strategy = BUILT_IN_STRATEGIES[i];
        }
    }
}

void to_json(json &j, const GameMaster &gm)
{
    j["max_energy"] = GameMaster::max_energy;
    j["min_stage"] = GameMaster::min_stage;
    j["max_stage"] = GameMaster::max_stage;
    j["dodge_duration"] = GameMaster::dodge_duration;
    j["dodge_window"] = GameMaster::dodge_window;
    j["swap_duration"] = GameMaster::swap_duration;
    j["switching_cooldown"] = GameMaster::switching_cooldown;
    j["rejoin_duration"] = GameMaster::rejoin_duration;
    j["item_menu_animation_time"] = GameMaster::item_menu_animation_time;
    j["max_revive_time_per_pokemon"] = GameMaster::max_revive_time_per_pokemon;
    j["same_type_attack_bonus_multiplier"] = GameMaster::same_type_attack_bonus_multiplier;
    j["weather_attack_bonus_multiplier"] = GameMaster::weather_attack_bonus_multiplier;
    j["pvp_fast_attack_bonus_multiplier"] = GameMaster::pvp_fast_attack_bonus_multiplier;
    j["pvp_charged_attack_bonus_multiplier"] = GameMaster::pvp_charged_attack_bonus_multiplier;
    j["dodge_damage_reduction_percent"] = GameMaster::dodge_damage_reduction_percent;
    j["energy_delta_per_health_lost"] = GameMaster::energy_delta_per_health_lost;

    std::vector<std::vector<double>> matrix;
    auto num_types = GameMaster::get_num_types();
    for (int i = 0; i < num_types; ++i)
    {
        std::vector<double> row;
        for (int j = 0; j < num_types; ++j)
        {
            row.push_back(GameMaster::get_effectiveness(i, j));
        }
        matrix.push_back(row);
    }
    j["type_effectiveness"] = matrix;

    std::vector<double> stage_multipliers;
    for (auto s = GameMaster::min_stage; s <= GameMaster::max_stage; ++s)
    {
        stage_multipliers.push_back(GameMaster::get_stage_multiplier(s));
    }
    j["stage_multipliers"] = stage_multipliers;

    std::vector<int> type_boosted_weather;
    for (int i = 0; i < num_types; ++i)
    {
        type_boosted_weather.push_back(GameMaster::get_type_boosted_weather(i));
    }
    j["type_boosted_weather"] = type_boosted_weather;
}

void from_json(const json &j, GameMaster &gm)
{
    GameMaster::max_energy = j["max_energy"];
    GameMaster::min_stage = j["min_stage"];
    GameMaster::max_stage = j["max_stage"];
    GameMaster::dodge_duration = j["dodge_duration"];
    GameMaster::dodge_window = j["dodge_window"];
    GameMaster::swap_duration = j["swap_duration"];
    GameMaster::switching_cooldown = j["switching_cooldown"];
    GameMaster::rejoin_duration = j["rejoin_duration"];
    GameMaster::item_menu_animation_time = j["item_menu_animation_time"];
    GameMaster::max_revive_time_per_pokemon = j["max_revive_time_per_pokemon"];
    GameMaster::same_type_attack_bonus_multiplier = j["same_type_attack_bonus_multiplier"];
    GameMaster::weather_attack_bonus_multiplier = j["weather_attack_bonus_multiplier"];
    GameMaster::pvp_fast_attack_bonus_multiplier = j["pvp_fast_attack_bonus_multiplier"];
    GameMaster::pvp_charged_attack_bonus_multiplier = j["pvp_charged_attack_bonus_multiplier"];
    GameMaster::dodge_damage_reduction_percent = j["dodge_damage_reduction_percent"];
    GameMaster::energy_delta_per_health_lost = j["energy_delta_per_health_lost"];

    auto matrix = j["type_effectiveness"].get<std::vector<std::vector<double>>>();
    auto num_types = matrix.size();
    GameMaster::set_num_types(num_types);
    for (int i = 0; i < num_types; ++i)
    {
        for (int j = 0; j < num_types; ++j)
        {
            GameMaster::set_effectiveness(i, j, matrix.at(i).at(j));
        }
    }

    auto stage_multipliers = j["stage_multipliers"].get<std::vector<double>>();
    GameMaster::set_stage_bounds(GameMaster::min_stage, GameMaster::max_stage);
    for (auto s = GameMaster::min_stage; s <= GameMaster::max_stage; ++s)
    {
        GameMaster::set_stage_multiplier(s, stage_multipliers.at(s - GameMaster::min_stage));
    }

    auto type_boosted_weather = j["type_boosted_weather"].get<std::vector<int>>();
    for (int i = 0; i < num_types; ++i)
    {
        GameMaster::set_type_boosted_weather(i, type_boosted_weather.at(i));
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
    j["type"] = action.type;
    j["delay"] = action.delay;
    j["value"] = action.value;
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
    j["type"] = event.type;
    j["delay"] = event.time;
    j["player"] = event.player;
    j["value"] = event.value;
}

void to_json(json &j, const PokemonState &pkm_st)
{
    j["active"] = pkm_st.active;
    j["immortal"] = pkm_st.immortal;
    j["max_hp"] = pkm_st.max_hp;
    j["hp"] = pkm_st.hp;
    j["energy"] = pkm_st.energy;
    j["damage_reduction_expiry"] = pkm_st.damage_reduction_expiry;
    j["tdo"] = pkm_st.tdo;
    j["tdo_fast"] = pkm_st.tdo_fast;
    j["duration"] = pkm_st.duration;
    j["num_deaths"] = pkm_st.num_deaths;
    j["num_fmoves_used"] = pkm_st.num_fmoves_used;
    j["num_cmoves_used"] = pkm_st.num_cmoves_used;
}

void to_json(json &j, const BattleOutcome &outcome)
{
    j["duration"] = outcome.duration;
    j["win"] = outcome.win;
    j["tdo"] = outcome.tdo;
    j["tdo_percent"] = outcome.tdo_percent;
    j["num_deaths"] = outcome.num_deaths;
}

}; // namespace GoBattleSim
