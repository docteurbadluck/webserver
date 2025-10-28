#include "EpollServerRunner.hpp"

void EpollServerRunner::run()
{
	while (this->running)
	{
		int n = wait_for_events();
		process_events(n);
		disconect_expired_client();
		this->session_handler.purge_expired_sessions();
	}
}

int	EpollServerRunner::wait_for_events()
{
	int n;
	while (true)
	{
		n = epoll_wait(this->epoll_fd, this->events, 64, 1000);
		if (n == -1)
		{
			if (errno == EINTR)
				continue;
			throw std::runtime_error("epoll_wait failed");
		}
		return n;
	}
}

void EpollServerRunner::process_events(int eventCount)
{
	for (int i = 0; i < eventCount; i++)
	{
		int fd = events[i].data.fd;
		if (is_an_accepting_socket(fd))
		{
			handle_new_connection(fd);
		}
		else if (events[i].data.fd == 0)
		{
			handle_command();
		}
		else
		{
			handle_message_from_client(clients[fd]);
		}
	}
}

bool EpollServerRunner::is_an_accepting_socket(int fd) const
{
	return (std::find(accepting_socket.begin(), accepting_socket.end(),
		fd) != accepting_socket.end());
}


void EpollServerRunner::handle_command()
{
	char	buffer[1024];
	ssize_t	count;
	std::string str;

	count = read(0, buffer, sizeof(buffer));
	buffer[count] = 0;
	str = buffer;
	if (str == "close\n")
	{
		std::cout <<"close command called." << std::endl;
		this->running = 0;
	}
	else
	{
		std::cout << "command not recognised" << std::endl;
	}
	
}