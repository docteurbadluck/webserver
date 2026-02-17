#include "catch.hpp"
#include <sys/socket.h>
#include <unistd.h>
#include <cerrno>
#include "StreamOut.hpp"
#include "IClient.hpp"
#include "Client.hpp"
#include <signal.h>
#include "StreamIn.hpp"

TEST_CASE("StreamOut returns STREAMOUT_CLOSE on write error") 
{
	signal(SIGPIPE, SIG_IGN);  
    int fds[2];
    REQUIRE(socketpair(AF_UNIX, SOCK_STREAM, 0, fds) == 0);

    // Crée un client
    IClient* client = new Client(fds[0], 1);
    client->stream.active = true;
    client->stream.offset = 0;
    client->stream.buffer = "Hello World";
    client->stream.close_after = false; // on veut que StreamOut détecte l'erreur
    client->stream.fd = -1;

    // Ferme le côté "serveur" du socket pour provoquer une erreur d'écriture
    close(fds[1]);

    StreamOut stream_out;

    // Appelle pump, il doit rencontrer une erreur d'écriture
    streamout_status st = stream_out.pump(client);

    REQUIRE(st == STREAMOUT_CLOSE);  // On attend que le client soit déconnecté

    delete client;
}

TEST_CASE("StreamIn returns STREAM_ERROR on read error") 
{

    // Crée un client avec un mauvais fd
    IClient* client = new Client(-1, 1);

    StreamIn stream_in;

    // Ferme le côté “peer” pour provoquer une erreur de lecture

    // Appelle read_from_fd, doit rencontrer une erreur
    stream_status_e st = stream_in.read_from_fd(client->get_fd(), *client);

    REQUIRE(st == STREAM_ERROR);  // On attend STREAM_ERROR
    delete client;
}
