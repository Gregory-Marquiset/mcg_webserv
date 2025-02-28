NAME := webserv

CXX := c++

CXXFLAGS += -Wall -Wextra -Werror -std=c++98 -g3

SRCS :=		main.cpp\
			srcs/configFile/Block.cpp\
			srcs/configFile/ReadyBlock.cpp\
			srcs/configFile/RecupBlockContent.cpp\
			srcs/server/ASocket.cpp\
			srcs/server/ListeningSocket.cpp\
			srcs/server/Server.cpp\
			srcs/epollManager/EPollManager.cpp\
			
OBJS := $(SRCS:.cpp=.o)

all: $(NAME)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(NAME): $(OBJS)
	$(CXX) $^ -o $@

clean:
	rm -f $(OBJS)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re