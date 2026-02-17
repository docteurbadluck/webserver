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

#include "InitEpoll.hpp"
#include "StreamIn.hpp"
#include "ConnectionManager.hpp"
#include "TimeoutManager.hpp"
#include "CommandManager.hpp"
#include "StreamOut.hpp"
#include "ModeSwitcher.hpp"

class EpollServerRunner : public IRunServer
{
	friend class InitEpoll;
	public :
		EpollServerRunner(	const t_server_rules &server_rules,
							SessionHandlerUC &session_uc,
							RequestHandlerUC &request_handler);
		~EpollServerRunner();
		void 						run();
		void 						init(std::vector<ISocketServer*> servers);
        void                        turn_off();
	private :
        const t_server_rules        &server_rules;
        SessionHandlerUC            &session_handler;
        RequestHandlerUC            &request_handler;
		CommandManager				command_manager;
		StreamIn					stream_in;
		StreamOut					stream_out;
		TimeoutManager				timeout_manager;
		ModeSwitcher				*mode_switcher;

		int							epoll_fd;
        std::map <int, IClient*>    clients;
        std::vector<int>            accepting_socket;
        int                         client_id;
        struct epoll_event          events[64];
        bool                        running;


        int                         wait_for_events();
        void                        process_events(int eventCount);
		void						handle_client_events(int fd, uint32_t events);

        void                        connect_new_client(int server_fd);
		int                         accept_connection(int server_fd);
        void                        set_socket_non_blocking(int fd);
        bool                        is_new_client(int fd) const;
		bool						is_keyboard_command(int fd);

        void                        handle_message_from_client(IClient *client);
        void                        handle_request(IClient *client);
		void						handle_requests(IClient *client);
		void						send_response_to_client(IClient *client);
      
		void                        disconnect_client(IClient *client);
};

