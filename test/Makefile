CXX = g++ 
FLAGS = -std=c++17
NAME = test_webserv
BUILD_DIR = build
SRCS = test_main.cpp test_2.cpp
OBJS = $(SRCS:%.cpp=$(BUILD_DIR)/%.o)

PROJECT_BUILD_DIR = ./../webserv/build
LIBRARY_NAME = $(BUILD_DIR)/libwebserver.a
PROJECT_OBJ = $(filter-out %/main.o,$(shell find $(PROJECT_BUILD_DIR) -name '*.o'))
PROJECT_INCLUDES = $(shell find ../webserv/srcs -type f -name '*hpp' -exec dirname {} \; | sort -u)
INCLUDES = $(sort $(PROJECT_INCLUDES))
INCDIRS = $(addprefix -I, $(INCLUDES))
LIB_STAMP = $(BUILD_DIR)/libwebserver.stamp

all: $(NAME)
	./test_webserv

$(LIBRARY_NAME): $(PROJECT_OBJ)
	@mkdir -p $(BUILD_DIR)
	ar rcs $@ $^

$(NAME): $(OBJS) $(LIBRARY_NAME) #construit les .o
	$(CXX) $(FLAGS) $(INCDIRS) $(OBJS) $(LIBRARY_NAME) -o $@
	rm -f $(LIBRARY_NAME)

#this line is called by OBJS
$(BUILD_DIR)/%.o: %.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(FLAGS)  $(INCDIRS) -c $< -o $@	#$< input in this case th file.cpp 

clean:
	rm -fr $(BUILD_DIR)

fclean: clean
	rm -fd $(NAME)

re: fclean all

.PHONY: all clean fclean re 
