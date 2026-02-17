#pragma once 
#include <map>
#include "IClient.hpp"
#include <vector>
#include "server_rules.hpp"
#include "ConnectionManager.hpp"
#include <cstring>
#include <sys/socket.h>

class TimeoutManager
{
public:
    int check_clients_timeout(
        std::map<int, IClient*> &clients,
        const t_server_rules &rules,
		const int &epoll_fd
    );

    int check_session_timeout(
        std::map<int, IClient*> &clients
    );

    void update_timestamp_last_activity(IClient *client);

private:
    std::vector<IClient*> list_clients_timed_out(
        std::map<int, IClient*> &clients,
		const t_server_rules &rules);

    int disconnect_timed_out_clients(
        std::vector<IClient*> &clients_to_disconnect,
		std::map<int, IClient *> &clients,
		const int &epoll_fd
    );
    
	void send_timeout_response(IClient *client);
};

