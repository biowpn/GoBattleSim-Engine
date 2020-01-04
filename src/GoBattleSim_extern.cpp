
#include "GoBattleSim_extern.h"
#include "GoBattleSim.h"
#include "json_converter.hpp"

#include <stdlib.h>

using namespace GoBattleSim;

class MessageCenter
{
public:
	static MessageCenter &get()
	{
		return instance;
	}

	void set_msg(const std::string &msg)
	{
		if (msg.size() >= m_msg_max_len)
		{
			delete[] m_msg;
			m_msg_max_len = msg.size();
			m_msg = new char[m_msg_max_len + 1];
		}
		strcpy(m_msg, msg.c_str());
	}

	const char *get_msg() const
	{
		return m_msg;
	}

private:
	MessageCenter() = default;
	static MessageCenter instance;

	char *m_msg{nullptr};
	unsigned m_msg_max_len{0};
};

MessageCenter MessageCenter::instance;

const char *GBS_version()
{
	return "0.8.0";
}

const char *GBS_error()
{
	return err_msg;
}

void GBS_prepare(const char *input_j)
{
	auto j = nlohmann::json::parse(input_j);
	auto mode = j.at("battleMode").get<BattleMode>();
	auto &app = GoBattleSimApp::get();

	if (mode == BattleMode::PvE)
	{
		PvESimInput input = j;
		app.prepare(input);
	}
	else if (mode == BattleMode::PvP)
	{
		PvPSimInput input = j;
		app.prepare(input);
	}
	else if (mode == BattleMode::BattleMatrix)
	{
		BattleMatrixSimInput input = j;
		app.prepare(input);
	}
	else
	{
		sprintf(err_msg, "impossible battle mode %d", (int)mode);
	}
}

void GBS_run()
{
	GoBattleSimApp::get().run();
}

template <class Output_t>
void collect_and_set(GoBattleSimApp &app, nlohmann::json &j)
{
	Output_t output;
	app.collect(output);
	j = output;
}

const char *GBS_collect()
{
	auto &app = GoBattleSimApp::get();
	nlohmann::json j;

	if (app.battle_mode == BattleMode::PvE)
	{
		if (app.aggregation_mode == AggregationMode::None)
		{
			collect_and_set<std::vector<PvEBattleOutcome>>(app, j);
		}
		else
		{
			collect_and_set<PvEAverageBattleOutcome>(app, j);
		}
	}
	else if (app.battle_mode == BattleMode::PvP)
	{
		if (app.aggregation_mode == AggregationMode::None)
		{
			collect_and_set<std::vector<SimplePvPBattleOutcome>>(app, j);
		}
		else
		{
			collect_and_set<SimplePvPBattleOutcome>(app, j);
		}
	}
	else if (app.battle_mode == BattleMode::BattleMatrix)
	{
		collect_and_set<Matrix_t>(app, j);
	}
	else
	{
		sprintf(err_msg, "impossible battle mode %d", (int)app.battle_mode);
	}

	auto j_str = j.dump(4);
	MessageCenter::get().set_msg(j_str);
	return MessageCenter::get().get_msg();
}

const char *GBS_config(const char *gm_j)
{
	if (gm_j != nullptr)
	{
		GameMaster::get() = nlohmann::json::parse(gm_j);
	}
	auto j_str = nlohmann::json(GameMaster::get()).dump(4);
	MessageCenter::get().set_msg(j_str);
	return MessageCenter::get().get_msg();
}
