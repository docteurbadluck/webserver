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
		int			get_fd() const;
		void		appendToReadBuffer(const std::string &data);
		const std::string	&getReadBuffer() const;
		bool 		delimiter_found_in_readbuffer();
		void		extract_header();
		void		flush_header();
		std::string	extract_body(int body_lenght);
		void		flush_body(int body_lenght);
		std::string	&get_request();
		void		update_last_activity();
		bool		is_timed_out(std::time_t now, int timeout);

	private :
		int			fd;
		int			id_client;
		std::string	write_buffer;
		std::string	read_buffer;
		std::string	delimiter;
		std::string	request;
		size_t		pos_delimiter;
		std::time_t	last_activity;
};