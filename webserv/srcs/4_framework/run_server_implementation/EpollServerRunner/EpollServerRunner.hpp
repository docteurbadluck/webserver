#pragma once 
#include "IRunServer.hpp"
#include "IClient.hpp"
#include "Client.hpp"
#include <map>
#include <sys/epoll.h>
#include <stdexcept>
#include <iostream>
#include <sys/socket.h>
#include <unistd.h>
#include <vector>
#include <cstring>
#include <fcntl.h>
#include <errno.h>
#include <algorithm>
#include "IRequestParser.hpp"
#include "RequestParser.hpp"
#include "framework.hpp"
#include "IRequestHandler.hpp"
#include "RouterHandler.hpp"
#include "RequestHandlerUC.hpp"
#include "SessionHandlerUC.hpp"

class EpollServerRunner : public IRunServer
{
	public :
		EpollServerRunner(	const t_server_rules &server_rules,
							SessionHandlerUC &session_uc,
							RequestHandlerUC &request_handler);
		~EpollServerRunner();
		void 						run();
		void 						init_listening_socket(
										std::vector<ISocketServer*> servers);

	private :
		const t_server_rules		&server_rules;

		SessionHandlerUC			&session_handler;
		RequestHandlerUC			&request_handler;
		std::map <int, IClient*>	clients;
		
		std::vector<int> 			accepting_socket;
		int 						epoll_fd;
		int 						client_id;
		struct epoll_event			events[64];
		bool						running;

		void 						handle_new_connection(int server_fd);
		void						set_socket_non_blocking(int fd);
		bool 						is_an_accepting_socket(int fd) const;
		void 						process_events(int eventCount);
		int							wait_for_events();
		void 						add_listening_socket(int fd);


		void						handle_request(IClient *client, const t_server_rules &server_rules);
		void 						handle_message_from_client(IClient *client);
		int							accept_connection(int server_fd);
		void						store_client_in_map(int client_fd);
		void						discard_client_from_map(int client_fd);

		void						add_client_fd_to_epoll(int client_fd);
		void 						remove_client_fd_from_epoll(int fd);

		void						disconnect_client(IClient *client);
		void						handle_received_data(IClient *client,
										 const char *buffer, ssize_t count);
		void						disconect_expired_client();
		void						send_timeout_to_client(IClient *c);

		void						handle_request_bodyless(IClient *client);
		bool						is_the_body_complete(const t_parsed_request &parsed_request, IClient *client);
		void						handle_request_with_body(IClient *client, t_parsed_request &parsed_request);
		void						parse_request_header(t_parsed_request	&parsed_request, IClient *client);
		void						add_stdin_fd();
		void						handle_command();
};
