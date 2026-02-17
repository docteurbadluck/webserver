#pragma once
#include "usecases.hpp"
#include "framework.hpp"

#include <stdexcept>


int             START_SERVER(char **argv);

//helper
int             init_rules_for_the_server(t_server_rules &server_rules, char *pathfile);
ServerBuilder   *build_the_server(const t_server_rules &server_rules, ISocketFactory &socket_factory);
void            run_the_server(ServerBuilder *server_builder, const t_server_rules &server_rules);
