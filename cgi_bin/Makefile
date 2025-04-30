SRCS := src/delete-session.cpp \
        src/session.cpp \
        src/signup.cpp \
        src/update-theme.cpp

CXX := c++
CXXFLAGS := -Wall -Wextra -Werror -std=c++98 -g3 -MMD -MP
INCLUDES := -I../includes
RM := rm -rf
MK := mkdir -p

YELLOW = \033[1;33m
GREEN = \033[1;32m
CYAN = \033[1;36m
RESET = \033[0m

DEPS_DIR := dep/

CGIS := $(patsubst src/%.cpp,%.cgi,$(SRCS))
DEPS := $(patsubst src/%.cpp,$(DEPS_DIR)%.d,$(SRCS))

all: $(CGIS)

%.cgi: src/%.cpp
	@echo "$(YELLOW)[CGI] Compiling $(RESET)$(CYAN)$<$(RESET)..."
	@$(MK) $(dir $(DEPS_DIR)$*)
	$(CXX) $(CXXFLAGS) $(INCLUDES) $< -o $@ -MF $(DEPS_DIR)$*.d

clean:
	@echo "$(YELLOW)Cleaning dependencies...$(RESET)"
	$(RM) $(DEPS_DIR)

fclean: clean
	@echo "$(YELLOW)Cleaning CGI executables...$(RESET)"
	$(RM) $(CGIS)

re: fclean all

-include $(DEPS)

.PHONY: all clean fclean re
