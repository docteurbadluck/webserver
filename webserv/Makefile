# Compiler and flags
CXX       := g++
CXXFLAGS  := -Wall -Wextra -Werror -std=c++98 -g -MMD -MP
RM        := rm -rf

# Project names
NAME      := webserv
TEST_NAME := unit_tests

# Directories
SRC_DIR      := srcs
BUILD_DIR    := build
TEST_DIR     := tests
TEST_OBJ_DIR := $(BUILD_DIR)/tests

# Includes
SRC_INCLUDES  := $(shell find $(SRC_DIR) -type f -name '*.hpp' -exec dirname {} \; | sort -u)
TEST_INCLUDES := $(shell find $(TEST_DIR) -type f -name '*.hpp' -exec dirname {} \; | sort -u)
INCLUDES     := $(sort $(SRC_INCLUDES) $(TEST_INCLUDES))
INCDIRS      := $(addprefix -I, $(INCLUDES))

# Source files
SRCS := \
	srcs/5_main/main.cpp \
	srcs/4_framework/init_rules_implementation/file_io/FileIO.cpp \
	srcs/4_framework/init_rules_implementation/ConfigReader/ConfigReader.cpp \
	srcs/4_framework/init_rules_implementation/ConfigReader/init_configuration_struct/ConfigReader_init_config_struct.cpp \
	srcs/4_framework/init_rules_implementation/ConfigReader/init_configuration_struct/ConfigReader_init_enable.cpp \
	srcs/4_framework/init_rules_implementation/ConfigReader/init_configuration_struct/ConfigReader_init_filepath.cpp \
	srcs/4_framework/init_rules_implementation/ConfigReader/init_configuration_struct/ConfigReader_init_number.cpp \
	srcs/4_framework/init_rules_implementation/ConfigReader/init_configuration_struct/ConfigReader_init_config_session.cpp \
	srcs/4_framework/init_rules_implementation/ConfigReader/ConfigReader_verify_tools.cpp \
	srcs/4_framework/init_rules_implementation/ConfigReader/ConfigReader_get_parsed_config.cpp \
	srcs/2_usescases/init_server_rules/RulesInitialiser/RulesInitialiser.cpp \
	srcs/4_framework/print_tools/print_server_rules.cpp \
	srcs/2_usescases/build_server/ServerBuilder.cpp \
	srcs/4_framework/build_server_implementation/socket_implementation/ServerSocket.cpp \
	srcs/4_framework/build_server_implementation/socket_implementation/bind_socket.cpp \
	srcs/4_framework/build_server_implementation/SocketFactory/SocketFactory.cpp \
	srcs/4_framework/handle_client_implementation/Client/Client.cpp \
	srcs/4_framework/handle_client_implementation/Client/ClientTools.cpp \
	srcs/4_framework/handle_client_implementation/Client/ClientExtract.cpp \
	srcs/4_framework/run_server_implementation/EpollServerRunner/EpollServerRunner.cpp \
	srcs/4_framework/run_server_implementation/EpollServerRunner/Epoll_run.cpp \
	srcs/4_framework/run_server_implementation/EpollServerRunner/Epoll_init.cpp \
	srcs/4_framework/run_server_implementation/EpollServerRunner/Epoll_handle_connection.cpp \
	srcs/4_framework/run_server_implementation/EpollServerRunner/Epoll_handle_client.cpp \
	srcs/4_framework/run_server_implementation/EpollServerRunner/Epoll_timeout.cpp \
	srcs/2_usescases/run_server/RunnerServer.cpp \
	srcs/4_framework/handle_client_implementation/request_implementation/RequestParser/RequestParser.cpp \
	srcs/4_framework/handle_client_implementation/request_implementation/RequestParser/read_component.cpp \
	srcs/4_framework/handle_client_implementation/request_implementation/RequestParser/init_field.cpp \
	srcs/4_framework/print_tools/print_parsed_request.cpp \
	srcs/4_framework/handle_client_implementation/request_implementation/Handler/RouterHandler.cpp \
	srcs/4_framework/handle_client_implementation/request_implementation/Handler/GetHandler/GetHandler.cpp \
	srcs/4_framework/handle_client_implementation/request_implementation/Handler/GetHandler/handle.cpp \
	srcs/4_framework/handle_client_implementation/request_implementation/Handler/GetHandler/build_http_response.cpp \
	srcs/2_usescases/handle_client/handle_request/RequestHandler/RequestHandlerUC.cpp \
	srcs/2_usescases/handle_client/handle_session/SesssionHandler/SessionHandlerUC.cpp \
	srcs/4_framework/handle_client_implementation/session_implementation/policy/SessionPolicyFramework.cpp \
	srcs/4_framework/handle_client_implementation/session_implementation/storage/SessionStorageFramework.cpp \
	srcs/4_framework/handle_client_implementation/request_implementation/Handler/BaseHandler/BaseHandler.cpp \
	srcs/4_framework/handle_client_implementation/request_implementation/Handler/BaseHandler/buid_file_path.cpp \
	srcs/4_framework/handle_client_implementation/request_implementation/Handler/DeleteHandler/DeleteHandler.cpp \
	srcs/4_framework/handle_client_implementation/request_implementation/Handler/PostHandler/PostUploadHandler/PostUploadHandler.cpp \
	srcs/4_framework/handle_client_implementation/request_implementation/Handler/PostHandler/PostLoginHandler/PostLoginHandler.cpp \
	srcs/4_framework/handle_client_implementation/request_implementation/Handler/PostHandler/PostLoginHandler/build_http_response_login.cpp \
	srcs/4_framework/handle_client_implementation/request_implementation/Handler/PostHandler/PostCloseHandler/PostCloseHandler.cpp \
	srcs/4_framework/handle_client_implementation/request_implementation/Handler/TrashHandler/TrashHandler.cpp \
	srcs/4_framework/handle_client_implementation/request_implementation/Handler/PostHandler/PostTrashHandler/PostTrashHandler.cpp
	
# Object files (in build/)
OBJS := $(SRCS:%.cpp=$(BUILD_DIR)/%.o)
DEPS := $(OBJS:.o=.d)

# Objects without main.cpp (for tests)
NON_MAIN_SRCS := $(filter-out $(SRC_DIR)/5_main/main.cpp, $(SRCS))
NON_MAIN_OBJS := $(NON_MAIN_SRCS:%.cpp=$(BUILD_DIR)/%.o)

# Test source and object files
TEST_SRCS := \
	tests/test.cpp 

TEST_OBJS := $(TEST_SRCS:%.cpp=$(BUILD_DIR)/%.o)

# Default target
all: $(NAME)

# Build main executable
$(NAME): $(OBJS)
	$(CXX) $(CXXFLAGS) $(INCDIRS) $(OBJS) -o $@

# Rule to compile source files into build/
$(BUILD_DIR)/%.o: %.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $(INCDIRS) -c $< -o $@

# Build and run tests
test: $(TEST_NAME)
	./$(TEST_NAME)

$(TEST_NAME): $(NON_MAIN_OBJS) $(TEST_OBJS)
	$(CXX) $(CXXFLAGS) $(INCDIRS) $^ -o $@

# Cleaning
clean:
	$(RM) $(BUILD_DIR)

fclean: clean
	$(RM) $(NAME) $(TEST_NAME)

re: fclean all

-include $(DEPS)

.PHONY: all clean fclean re test
