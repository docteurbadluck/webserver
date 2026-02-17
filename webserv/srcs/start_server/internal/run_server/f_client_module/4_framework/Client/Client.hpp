#pragma once
#include "IClient.hpp"
#include <string>
#include <sys/types.h>
#include <sys/socket.h>



class Client : public IClient
{
	public :
		~Client();
		Client(int fd, int id_client);
		
		//epollin

		int			get_fd() const;
		void		update_last_activity();
		bool		is_timed_out(std::time_t now, int timeout);

		
	private :
		int			fd;
		int			id_client;
		std::time_t	last_activity;


};
