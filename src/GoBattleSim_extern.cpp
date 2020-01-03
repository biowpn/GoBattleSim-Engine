
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
	SimInput input = nlohmann::json::parse(input_j);
	// for now, hardcode to no aggregation
	input.aggreation = AggregationMode::None;
	GoBattleSimApp::get().prepare(input);
}

void GBS_run()
{
	GoBattleSimApp::get().run();
}

const char *GBS_collect()
{
	// for now, hardcode to get last output
	SimOutput output;
	GoBattleSimApp::get().collect(output);
	auto j_str = nlohmann::json(output).dump(4);
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
