SRCS :=		main.cpp\
			srcs/configFile/Block.cpp\
			srcs/configFile/RecupBlockContent.cpp\
			srcs/configFile/LocationBlock.cpp\
			srcs/configFile/ServerBlock.cpp\
			srcs/configFile/CgiHandler.cpp\
			srcs/configFile/HostHandler.cpp\
			srcs/server/ASocket.cpp\
			srcs/server/ListeningSocket.cpp\
			srcs/server/Server.cpp\
			srcs/epollManager/EPollManager.cpp\
			srcs/request/RequestParser.cpp\
			srcs/request/ProcessRequest.cpp\
			srcs/errorManagement/ErrorManagement.cpp\
			srcs/response/ResponseMaker.cpp\
			srcs/response/ResponseMakerPostCGI.cpp\
			srcs/utils/Utils.cpp\
			srcs/cgi/cgi.cpp

NAME := webserv
INCLUDES = -Iincludes
CXX := c++
CXXFLAGS += -Wall -Wextra -Werror -std=c++98 -g3 -MMD -MP
RM = rm -rf
MK = mkdir -p

YELLOW = \033[1;33m
GREEN = \033[1;32m
CYAN = \033[1;36m
RESET = \033[0m

UP_DIR = www/upload/
SESS_DIR = www/sessions_bin/
OBJS_DIR = obj/
DEPS_DIR = dep/
OBJS = $(SRCS:.cpp=.o)
OBJS_PREF = $(addprefix $(OBJS_DIR), $(OBJS))

DEPENDENCIES = $(OBJS_PREF:.o=.d)

all: $(NAME)
	@$(MK) $(UP_DIR)
	@$(MK) $(SESS_DIR)

$(OBJS_DIR)%.o: %.cpp
	@echo "$(YELLOW)Compiling $(RESET)$(CYAN)$<$(RESET)..."
	@$(MK) $(dir $@) $(DEPS_DIR)$(dir $<)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@ -MMD -MF $(DEPS_DIR)$*.d

$(NAME): $(OBJS_PREF)
	@echo "$(YELLOW)Linking $(CYAN)$(NAME)$(RESET)..."
	$(CXX) $(CXXFLAGS) $(OBJS_PREF) -o $(NAME)
	@echo "$(GREEN)Executable $(CYAN)$(NAME)$(GREEN) successfully created!$(RESET)"

clean:
	@echo "$(YELLOW)Cleaning object files, dependencies $(CYAN)$(OBJS_DIR) $(DEPS_DIR) $(RESET)..."
	$(RM) $(OBJS_DIR)
	$(RM) $(DEPS_DIR)

fclean: clean
	@echo "$(YELLOW)Removing executable and utils directories $(CYAN)$(NAME) $(UP_DIR) $(SESS_DIR)$(RESET)..."
	$(RM) $(NAME)
	$(RM) $(UP_DIR)
	$(RM) $(SESS_DIR)

re: fclean all

-include $(DEPENDENCIES)

.PHONY: all clean fclean re
