#include "CommandManager.hpp"
#include "EpollServerRunner.hpp"

CommandManager::CommandManager()
{
    command_list["help\n"]  = &CommandManager::cmd_help;
    command_list["close\n"] = &CommandManager::cmd_close;
}

std::string CommandManager::read_command()
{
	char    buffer[1024];
    ssize_t count;
    std::string str;

    count = read(0, buffer, sizeof(buffer));
    buffer[count] = 0;
    str = buffer;
	return str;
}


bool CommandManager::execute_command(
    EpollServerRunner &runner)
{
    std::map<std::string, CommandFunction>::iterator it;
	std::string command;
	
	command = read_command();
    it = command_list.find(command);
    if (it == command_list.end())
    {
        std::cout << "command not recognised" << std::endl;
        return false;
    }

    return (this->*(it->second))(runner);
}


bool CommandManager::cmd_close(EpollServerRunner &runner)
{
    std::cout << "close command called." << std::endl;
    runner.turn_off();
    return true;
}


bool CommandManager::cmd_help(EpollServerRunner &)
{
    std::cout << "Available commands:" << std::endl;
    for (std::map<std::string, CommandFunction>::iterator it = command_list.begin();
         it != command_list.end(); ++it)
    {
        std::cout << " - " << it->first << std::endl;
    }
    return true;
}

