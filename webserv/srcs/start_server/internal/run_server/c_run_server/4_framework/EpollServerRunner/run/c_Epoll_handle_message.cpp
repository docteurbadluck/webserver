#include "EpollServerRunner.hpp"

void EpollServerRunner::handle_message_from_client(IClient *client)
{
    stream_status_e st = stream_in.read_from_fd(
        client->get_fd(),
        *client
    );

    if (st == STREAM_DISCONNECT || st == STREAM_ERROR)
    {
        disconnect_client(client);
        return;
    }

    stream_in.parse_from_overflow(*client);
	handle_requests(client);
    client->raw_request_completed.clear();
}
