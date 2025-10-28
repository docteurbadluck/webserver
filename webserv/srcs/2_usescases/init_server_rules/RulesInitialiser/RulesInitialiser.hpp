#pragma once
#include "server_config.hpp"
#include "server_rules.hpp"

class IConfigReader;

class RulesInitialiser
{
	public :
		explicit	RulesInitialiser(IConfigReader &config_reader);
		void		init_server_rules(t_server_rules &serverRules);

	private :
		void		pass_data_from_config_to_rules(
						const t_server_config &config,
						t_server_rules &server_rules);
		IConfigReader &reader;
};