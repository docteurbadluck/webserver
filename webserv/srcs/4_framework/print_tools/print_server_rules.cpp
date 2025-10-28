
#include <iostream>
#include <vector>
#include <string>
#include <utility> // for std::pair
#include "server_rules.hpp"
#include "framework.hpp"

// Codes ANSI simples pour couleurs
#define RESET_COLOR "\033[0m"
#define RED_COLOR "\033[31m"
#define GREEN_COLOR "\033[32m"
#define CYAN_COLOR "\033[36m"
#define YELLOW_COLOR "\033[33m"
#define MAGENTA_COLOR "\033[35m"
#define BLUE_COLOR "\033[34m"

void print_server_rules(const t_server_rules &rules)
{
    std::cout << BLUE_COLOR << "=== Server Rules ===" << RESET_COLOR << std::endl;

    std::cout << CYAN_COLOR << "Enable directory listing: " 
              << (rules.enable_directory_listing ? GREEN_COLOR "true" : RED_COLOR "false") 
              << RESET_COLOR << std::endl;

    std::cout << CYAN_COLOR << "Enable upload: " 
              << (rules.enable_upload ? GREEN_COLOR "true" : RED_COLOR "false") 
              << RESET_COLOR << std::endl;

    std::cout << CYAN_COLOR << "Backlog: " << YELLOW_COLOR << rules.backlog << RESET_COLOR << std::endl;

    std::cout << CYAN_COLOR << "IP/Port pairs:" << RESET_COLOR << std::endl;
    for (std::vector<std::pair<unsigned long, int> >::const_iterator it = rules.ip_port_vector.begin();
         it != rules.ip_port_vector.end(); ++it)
    {
        std::cout << "  " << MAGENTA_COLOR << "IP: " << it->first << RESET_COLOR
                  << " Port: " << YELLOW_COLOR << it->second << RESET_COLOR << std::endl;
    }

    std::cout << CYAN_COLOR << "Error page filepath: " << GREEN_COLOR << rules.error_page_filepath << RESET_COLOR << std::endl;
    std::cout << CYAN_COLOR << "Max size request body: " << YELLOW_COLOR << rules.max_size_request_body << RESET_COLOR << std::endl;

    std::cout << CYAN_COLOR << "Allowed methods:" << RESET_COLOR << std::endl;
    for (std::vector<std::string>::const_iterator it = rules.list_allowed_method.begin();
         it != rules.list_allowed_method.end(); ++it)
    {
        std::cout << "  " << GREEN_COLOR << *it << RESET_COLOR << std::endl;
    }

    std::cout << CYAN_COLOR << "Redirections:" << RESET_COLOR << std::endl;
    for (std::vector<std::pair<std::string, std::string> >::const_iterator it = rules.redirection.begin();
         it != rules.redirection.end(); ++it)
    {
        std::cout << "  " << MAGENTA_COLOR << "From: " << RESET_COLOR << GREEN_COLOR << it->first 
                  << RESET_COLOR << " To: " << GREEN_COLOR << it->second << RESET_COLOR << std::endl;
    }

    std::cout << CYAN_COLOR << "File system root: (" << GREEN_COLOR << rules.file_system_root.first
              << RESET_COLOR << ", " << GREEN_COLOR << rules.file_system_root.second << RESET_COLOR << ")" << std::endl;

    std::cout << CYAN_COLOR << "Upload folder: " << GREEN_COLOR << rules.upload_folder << RESET_COLOR << std::endl;
    std::cout << CYAN_COLOR << "Default filepath: " << GREEN_COLOR << rules.default_filepath << std::endl;

    std::cout << CYAN_COLOR << "CGI pathfiles:" << RESET_COLOR << std::endl;
    for (std::vector<std::string>::const_iterator it = rules.CGI_pathfile.begin();
         it != rules.CGI_pathfile.end(); ++it)
    {
        std::cout << "  " << GREEN_COLOR << *it << RESET_COLOR << std::endl;
    }

    std::cout << BLUE_COLOR << "====================" << RESET_COLOR << std::endl;
}
