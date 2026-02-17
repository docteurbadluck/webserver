#pragma once
#include <string>
#include <map>

class EpollServerRunner;

class CommandManager
{
public:
    CommandManager();
    bool execute_command(EpollServerRunner &runner);

private:
    typedef bool (CommandManager::*CommandFunction)(EpollServerRunner &);

    std::map<std::string, CommandFunction> command_list;
	std::string read_command();
    bool cmd_help(EpollServerRunner &runner);
    bool cmd_close(EpollServerRunner &runner);
};

