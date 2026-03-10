CXX       := g++
CXXFLAGS  := -Wall -Wextra -Werror -std=c++98 -g -MMD -MP
RM        := rm -rf

NAME      := webserv
SRC_DIR      := srcs
BUILD_DIR    := build

SRC_INCLUDES  := $(shell find $(SRC_DIR) -type f -name '*.hpp' -exec dirname {} \; | sort -u)
INCLUDES     := $(sort $(SRC_INCLUDES) $(TEST_INCLUDES))
INCDIRS      := $(addprefix -I, $(INCLUDES))

# Source files

SRC_USECASES = \
srcs/start_server/internal/init_server/a_init_server_rules/2_usescases/RulesInitialiser/RulesInitialiser.cpp \
srcs/start_server/internal/init_server/b_build_server/2_usescases/ServerBuilder/ServerBuilder.cpp \
\
srcs/start_server/internal/run_server/c_run_server/2_usescases/RunnerServer/RunnerServer.cpp \
srcs/start_server/internal/run_server/d_handle_request/2_usescases/RequestHandler/RequestHandlerUC.cpp \
srcs/start_server/internal/run_server/e_handle_session/2_usescases/SesssionHandler/SessionHandlerUC.cpp 

SRC_FRAMEWORK = \
srcs/start_server/internal/init_server/a_init_server_rules/4_framework/ConfigReader/ConfigReader.cpp \
srcs/start_server/internal/init_server/a_init_server_rules/4_framework/ConfigReader/ConfigReader_get_parsed_config.cpp \
srcs/start_server/internal/init_server/a_init_server_rules/4_framework/ConfigReader/internal/ConfigReader_verify_tools.cpp \
srcs/start_server/internal/init_server/a_init_server_rules/4_framework/ConfigReader/internal/ConfigReader_verify.cpp \
srcs/start_server/internal/init_server/a_init_server_rules/4_framework/ConfigReader/internal/init_configuration_struct/ConfigReader_init_config_session.cpp \
srcs/start_server/internal/init_server/a_init_server_rules/4_framework/ConfigReader/internal/init_configuration_struct/ConfigReader_init_config_struct.cpp \
srcs/start_server/internal/init_server/a_init_server_rules/4_framework/ConfigReader/internal/init_configuration_struct/ConfigReader_init_enable.cpp \
srcs/start_server/internal/init_server/a_init_server_rules/4_framework/ConfigReader/internal/init_configuration_struct/ConfigReader_init_filepath.cpp \
srcs/start_server/internal/init_server/a_init_server_rules/4_framework/ConfigReader/internal/init_configuration_struct/ConfigReader_init_number.cpp \
srcs/start_server/internal/init_server/a_init_server_rules/4_framework/file_io/FileIO.cpp \
\
srcs/start_server/internal/init_server/b_build_server/4_framework/ServerSocket/bind_socket.cpp \
srcs/start_server/internal/init_server/b_build_server/4_framework/ServerSocket/ServerSocket.cpp \
srcs/start_server/internal/init_server/b_build_server/4_framework/SocketFactory/SocketFactory.cpp \
\
srcs/start_server/internal/run_server/c_run_server/4_framework/EpollServerRunner/a_Epoll_run.cpp \
srcs/start_server/internal/run_server/c_run_server/4_framework/EpollServerRunner/b_Epoll_handle_connection.cpp \
srcs/start_server/internal/run_server/c_run_server/4_framework/EpollServerRunner/c_Epoll_handle_message.cpp \
srcs/start_server/internal/run_server/c_run_server/4_framework/EpollServerRunner/d_Epoll_handlers.cpp \
srcs/start_server/internal/run_server/c_run_server/4_framework/EpollServerRunner/e_Epoll_send_response_to_client.cpp \
srcs/start_server/internal/run_server/c_run_server/4_framework/EpollServerRunner/internal/Epoll_init.cpp \
srcs/start_server/internal/run_server/c_run_server/4_framework/EpollServerRunner/EpollServerRunner.cpp \
srcs/start_server/internal/run_server/c_run_server/4_framework/EpollServerRunner/internal/Epoll_disconect.cpp\
srcs/start_server/internal/run_server/c_run_server/4_framework/EpollServerRunner/internal/InitEpoll/InitEpoll.cpp\
srcs/start_server/internal/run_server/c_run_server/4_framework/EpollServerRunner/internal/StreamIn/StreamIn.cpp\
srcs/start_server/internal/run_server/c_run_server/4_framework/EpollServerRunner/internal/ConnectionManager/ConnectionManager.cpp\
srcs/start_server/internal/run_server/c_run_server/4_framework/EpollServerRunner/internal/TimeoutManager/TimeoutManager.cpp\
srcs/start_server/internal/run_server/c_run_server/4_framework/EpollServerRunner/internal/CommandManager/CommandManager.cpp\
srcs/start_server/internal/run_server/c_run_server/4_framework/EpollServerRunner/internal/StreamOut/StreamOut.cpp\
srcs/start_server/internal/run_server/c_run_server/4_framework/EpollServerRunner/internal/ModeSwitcher/ModeSwitcher.cpp\
\
srcs/start_server/internal/run_server/d_handle_request/4_framework/Handler/BaseHandler/BaseHandler.cpp \
srcs/start_server/internal/run_server/d_handle_request/4_framework/Handler/BaseHandler/buid_file_path.cpp \
srcs/start_server/internal/run_server/d_handle_request/4_framework/Handler/DeleteHandler/DeleteHandler.cpp \
srcs/start_server/internal/run_server/d_handle_request/4_framework/Handler/GetHandler/GetHandler.cpp \
srcs/start_server/internal/run_server/d_handle_request/4_framework/Handler/GetHandler/handle.cpp \
srcs/start_server/internal/run_server/d_handle_request/4_framework/Handler/GetHandler/build_http_response.cpp \
srcs/start_server/internal/run_server/d_handle_request/4_framework/Handler/PostHandler/PostCloseHandler/PostCloseHandler.cpp \
srcs/start_server/internal/run_server/d_handle_request/4_framework/Handler/PostHandler/PostLoginHandler/PostLoginHandler.cpp \
srcs/start_server/internal/run_server/d_handle_request/4_framework/Handler/PostHandler/PostLoginHandler/build_http_response_login.cpp \
srcs/start_server/internal/run_server/d_handle_request/4_framework/Handler/PostHandler/PostTrashHandler/PostTrashHandler.cpp \
srcs/start_server/internal/run_server/d_handle_request/4_framework/Handler/PostHandler/PostUploadHandler/PostUploadHandler.cpp \
srcs/start_server/internal/run_server/d_handle_request/4_framework/Handler/RedirectHandler/RedirectHandler.cpp \
srcs/start_server/internal/run_server/d_handle_request/4_framework/Handler/CGIHandler/CGIHandler.cpp \
srcs/start_server/internal/run_server/d_handle_request/4_framework/Handler/RouterHandler.cpp \
srcs/start_server/internal/run_server/d_handle_request/4_framework/Handler/TrashHandler/TrashHandler.cpp \
srcs/start_server/internal/run_server/d_handle_request/4_framework/Handler/MethodNotAllowedHandler/MethodNotAllowedHandler.cpp \
srcs/start_server/internal/run_server/d_handle_request/4_framework/Handler/InternalErrorHandler/InternalErrorHandler.cpp \
srcs/start_server/internal/run_server/d_handle_request/4_framework/RequestParser/internal/init_field.cpp \
srcs/start_server/internal/run_server/d_handle_request/4_framework/RequestParser/internal/read_component.cpp \
srcs/start_server/internal/run_server/d_handle_request/4_framework/RequestParser/RequestParser.cpp \
srcs/start_server/internal/run_server/d_handle_request/4_framework/RequestParser/RequestParser_parse_txt_request.cpp \
\
srcs/start_server/internal/run_server/e_handle_session/4_framework/policy/SessionPolicyFramework.cpp \
srcs/start_server/internal/run_server/e_handle_session/4_framework/storage/SessionStorageFramework.cpp \
srcs/start_server/internal/run_server/f_client_module/4_framework/Client/Client.cpp \
srcs/start_server/internal/run_server/f_client_module/4_framework/Client/ClientExtract.cpp \
srcs/start_server/internal/run_server/f_client_module/4_framework/Client/ClientTools.cpp \


SRC_MAIN = srcs/start_server/start_server.cpp \
           srcs/main/main.cpp

SRCS = $(SRC_USECASES) $(SRC_INTERFACES) $(SRC_FRAMEWORK) $(SRC_MAIN)

OBJS := $(SRCS:%.cpp=$(BUILD_DIR)/%.o)
DEPS := $(OBJS:.o=.d)

# Default target
all: $(NAME)

# Build main executable
$(NAME): $(OBJS)
	$(CXX) $(CXXFLAGS) $(INCDIRS) $(OBJS) -o $@

# Rule to compile source files into build/
$(BUILD_DIR)/%.o: %.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $(INCDIRS) -c $< -o $@

# Cleaning
clean:
	$(RM) $(BUILD_DIR)

fclean: clean
	$(RM) $(NAME) 

re: fclean all

-include $(DEPS)

.PHONY: all clean fclean re
