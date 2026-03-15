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
    // return true = stream terminé
    // return false = encore en cours (EPOLLOUT)
    streamout_status	pump(IClient *client);

private:
	streamout_status send_buffer(IClient*, StreamState&);
	streamout_status send_file(IClient*, StreamState&);
	void	prepare_next_response(IClient *client);
};

