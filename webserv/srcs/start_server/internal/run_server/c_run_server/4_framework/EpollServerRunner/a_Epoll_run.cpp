#include "EpollServerRunner.hpp"

void EpollServerRunner::run()
{
	while (this->running)
	{
		try
		{
			int n = wait_for_events();
			process_events(n);
			this->timeout_manager.check_clients_timeout(
						this->clients, this->server_rules, this->epoll_fd);
			this->session_handler.purge_expired_sessions();
		}
		catch (const std::exception &e)
		{
			std::cerr << "Event loop error: " << e.what() << std::endl;
		}
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
		if (is_new_client(fd))
		{
			connect_new_client(fd);
		}
		else if (is_keyboard_command(fd))
		{
			this->command_manager.execute_command(*this);
		}
		else
		{
			handle_client_events(fd, events[i].events);
		}
	}
}

void EpollServerRunner::handle_client_events(int fd, uint32_t events)
{
	if (this->mode_switcher->is_read_event(events))
    {
        handle_message_from_client(clients[fd]);
    }
    else if (this->mode_switcher->is_write_event(events))
    {
		send_response_to_client(clients[fd]);
    }
}
