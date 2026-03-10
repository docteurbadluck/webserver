Hey jackob, i hope you are alrigth :

run RUN_ME_ONCE.sh

then 
    cd webserv; make; ./webserv config.conf;

in another terminal:
    cd testeur_webserver; make; ./testeur_webserver;

all test should be succeed. if not, hit me up ^^

that the place you have to work:
webserv/srcs/start_server/internal/run_server/d_handle_request/4_framework/Handler/CGIHandler
even more in build_http_response for now it just look if the file is inside and return the appropriate error code.
this module is not that clean if i'm totally honest ^^
I spend looong time to clean the rest of the code so i have'nt the time for this one. if you want to rework,
Base handler can be optimized to be reuse better. with the test, it is easy to see if you broke something.
(for now 403 do not work anymore. i don't know why, not a big deal for now)
please add some test for your part (it's required by the subject to use our own test)
i think it's enough for now, 

if you need to handle stream : you have to implement this one for your handler.
{
    return handlers["GET"]->get_fd_stream();
}