
#include "StdAfx.h"
#include "Config.h"

CPhatACServerConfig *g_pConfig = NULL;

CKeyValueConfig::CKeyValueConfig(const char *filepath) : m_strFile(filepath)
{
}

CKeyValueConfig::~CKeyValueConfig()
{
	Destroy();
}

void CKeyValueConfig::Destroy()
{
	m_KeyValues.clear();
}

bool CKeyValueConfig::Load(const char *filepath)
{
	m_strFile = filepath;
	return Load();
}

bool CKeyValueConfig::Load()
{
	Destroy();

	std::ifstream f;
	f.open(m_strFile.c_str(), std::ios::in);

	if (f.fail())
		return false;

	std::string line;
	std::string valuename, value;
	std::string::size_type pLeft;

	while (getline(f, line))
	{
		if (!line.length())
			continue;

		// Standardize line endings
		if (line[line.length() - 1] == '\r')
			line = line.substr(0, line.length() - 1);

		if ((pLeft = line.find_first_of(";#=/")) != std::string::npos)
		{
			switch (line[pLeft])
			{
			case '=':
				valuename = line.substr(0, pLeft);
				value = line.substr(pLeft + 1);
				m_KeyValues[valuename] = value;

				break;
			case ';':
			case '#':
			case '/':
				// comments
				break;
			}
		}
	}

	f.close();
	
	PostLoad();
	return true;
}

const char *CKeyValueConfig::GetValue(const char *valuename, const char *defaultvalue)
{
	std::map<std::string, std::string>::iterator entry = m_KeyValues.find(valuename);

	if (entry != m_KeyValues.end())
		return entry->second.c_str();

	return defaultvalue;
}

CPhatACServerConfig::CPhatACServerConfig(const char *filepath) : CKeyValueConfig(filepath)
{
}

CPhatACServerConfig::~CPhatACServerConfig()
{
}

void CPhatACServerConfig::PostLoad()
{
	m_BindIP = inet_addr(GetValue("bind_ip", "127.0.0.1"));
	m_BindPort = strtoul(GetValue("bind_port", "9050"), NULL, 10);

	m_DatabaseIP = GetValue("database_ip", "127.0.0.1");
	m_DatabasePort = strtoul(GetValue("database_port", "0"), NULL, 10);
	m_DatabaseUsername = GetValue("database_username", "root");
	m_DatabasePassword = GetValue("database_password", "");
	m_DatabaseName = GetValue("database_name", "phatac");

	m_WorldName = GetValue("world_name", "");
	m_WelcomePopup = GetValue("welcome_popup", "");
	m_WelcomeMessage = GetValue("welcome_message", "");

	m_bFastTick = atoi(GetValue("fast_tick", "0")) != 0;

	m_bHardcoreMode = atoi(GetValue("hardcore_mode", "0")) != 0;
	m_bHardcoreModePlayersOnly = atoi(GetValue("hardcore_mode_players_only", "0")) != 0;
	m_bPKOnly = atoi(GetValue("player_killer_only", "0")) != 0;
	m_bColoredSentinels = atoi(GetValue("colored_sentinels", "0")) != 0;
	m_bSpawnLandscape = atoi(GetValue("spawn_landscape", "1")) != 0;
	m_bSpawnStaticCreatures = atoi(GetValue("spawn_static_creatures", "1")) != 0;
	m_bEverythingUnlocked = atoi(GetValue("everything_unlocked", "0")) != 0;
	m_bTownCrierBuffs = atoi(GetValue("town_crier_buffs", "0")) != 0;

	m_bEnableTeleCommands = atoi(GetValue("enable_teleport_commands", "0")) != 0;
	m_bEnableXPCommands = atoi(GetValue("enable_xp_commands", "0")) != 0;
	m_bEnableAttackableCommand = atoi(GetValue("enable_attackable_command", "0")) != 0;
	m_bEnableGodlyCommand = atoi(GetValue("enable_godly_command", "0")) != 0;

	m_fQuestMultiplierLessThan1Day = atof(GetValue("quest_time_multiplier_less_than_1_day", "1.0"));
	m_fQuestMultiplier1Day = atof(GetValue("quest_time_multiplier_1_day", "1.0"));
	m_fQuestMultiplier3Day = atof(GetValue("quest_time_multiplier_3_day", "1.0"));
	m_fQuestMultiplier7Day = atof(GetValue("quest_time_multiplier_7_day", "1.0"));
	m_fQuestMultiplier14Day = atof(GetValue("quest_time_multiplier_14_day", "1.0"));
	m_fQuestMultiplier30Day = atof(GetValue("quest_time_multiplier_30_day", "1.0"));
	m_fQuestMultiplier60Day = atof(GetValue("quest_time_multiplier_60_day", "1.0"));
	
	m_fKillXPMultiplier = max(0.0, atof(GetValue("kill_xp_multiplier", "1.0")));
	m_fRewardXPMultiplier = max(0.0, atof(GetValue("reward_xp_multiplier", "1.0")));

	double dropRate = max(0.0, atof(GetValue("drop_rate_multiplier", "4.0")));
	m_fDropRateMultiplier = (dropRate > 0.0) ? (1.0 / m_fDropRateMultiplier) : 0.0; // invert the drop rate for internal use

	m_bAutoCreateAccounts = atoi(GetValue("auto_create_accounts", "1")) != 0;

	m_MaxDormantLandblocks = (unsigned int)max(0, atoi(GetValue("max_dormant_landblocks", "1000")));
	m_DormantLandblockCleanupTime = (unsigned int)max(0, atoi(GetValue("dormant_landblock_cleanup_time", "1800")));

	m_bShowLogins = atoi(GetValue("show_logins", "1")) != 0;
	m_bSpeedHackKicking = atoi(GetValue("speed_hack_kicking", "1")) != 0;
	m_bShowDeathMessagesGlobally = atoi(GetValue("show_death_messages_globally", "0")) != 0;
	m_bShowPlayerDeathMessagesGlobally = atoi(GetValue("show_player_death_messages_globally", "0")) != 0;

	m_HoltburgStartPosition = GetValue("holtburg_start_position", "");
	m_YaraqStartPosition = GetValue("yaraq_start_position", "");
	m_ShoushiStartPosition = GetValue("shoushi_start_position", "");
	m_SanamarStartPosition = GetValue("samanar_start_position", "");

	m_PKRespiteTime = atoi(GetValue("pk_respite_time", "300"));
}

double CPhatACServerConfig::GetMultiplierForQuestTime(int questTime)
{
	if (questTime < 0)
		return 1.0;

	int days = (int)(questTime / 86400);

	if (days >= 60)
		return m_fQuestMultiplier60Day;
	if (days >= 30)
		return m_fQuestMultiplier30Day;
	if (days >= 14)
		return m_fQuestMultiplier14Day;
	if (days >= 7)
		return m_fQuestMultiplier7Day;
	if (days >= 3)
		return m_fQuestMultiplier3Day;
	if (days >= 1)
		return m_fQuestMultiplier1Day;
	
	return m_fQuestMultiplierLessThan1Day;
}

int CPhatACServerConfig::UseMultiplierForQuestTime(int questTime)
{
	return (int)(questTime * GetMultiplierForQuestTime(questTime));
}

