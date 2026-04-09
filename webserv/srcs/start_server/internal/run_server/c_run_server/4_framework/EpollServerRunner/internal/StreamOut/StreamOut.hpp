#pragma once

#include <string>
#include <unistd.h>
#include "IClient.hpp"
#include <sys/types.h>
#include <sys/stat.h>

enum streamout_status
{
    STREAMOUT_DONE,      // tout envoyé, rien d’autre à faire
    STREAMOUT_CONTINUE,  // reste des données à envoyer
    STREAMOUT_CLOSE,      // réponse terminée et connexion à fermer
	STREAMOUT_ERROR
};

class StreamOut
{
public:
    streamout_status    pump(IClient *client);

private:
    streamout_status    send_buffer(IClient*, StreamState&);
    streamout_status    send_file(IClient*, StreamState&);
    streamout_status    send_fd_chunk(IClient*, StreamState&, char*, ssize_t);
    streamout_status    finalize_stream(IClient*, StreamState&);
    streamout_status    finalize_fd(StreamState&);
    streamout_status    timeout_cgi(StreamState&);
    bool                is_cgi_timed_out(StreamState&);
    void                reap_cgi(StreamState&);
    void                prepare_next_response(IClient*);
};

