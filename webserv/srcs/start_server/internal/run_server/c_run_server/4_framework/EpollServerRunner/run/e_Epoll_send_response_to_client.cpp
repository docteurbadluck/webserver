#include "EpollServerRunner.hpp"

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
