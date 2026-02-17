#pragma once
#include "FileIO.hpp"
#include "ConfigReader.hpp"
#include "ServerSocket.hpp"
#include "SocketFactory.hpp"
#include "EpollServerRunner.hpp"
#include "parsed_request.hpp"
#include "SessionPolicyFramework.hpp"
#include "SessionStorageFramework.hpp"

void	print_parsed_request(const t_parsed_request &req);
void	print_server_rules(const t_server_rules &rules);
