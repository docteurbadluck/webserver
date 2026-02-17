#include "EpollServerRunner.hpp"

/*
void EpollServerRunner::send_response_to_client(IClient *client)
{
    if (!client)
        return;
    bool fully_sent = this->stream_out.pump(client);
    if (!fully_sent)
        return;
    this->mode_switcher->switch_to_read_mode(client);
	//if (client->answer_from_server.close_flag)
	//	disconnect_client(client);
}*/

void EpollServerRunner::send_response_to_client(IClient *client)
{
    streamout_status st = stream_out.pump(client);

    if (st == STREAMOUT_CLOSE || st == STREAMOUT_ERROR)
    {
        disconnect_client(client);
        return;
    }

    if (st == STREAMOUT_DONE)
    {
        mode_switcher->switch_to_read_mode(client);
    }
}
