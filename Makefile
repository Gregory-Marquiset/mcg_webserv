NAME := webserv

CXX := c++

CXXFLAGS += -Wall -Wextra -Werror -std=c++98 -g3

SRCS :=		main.cpp\
			srcs/socketInit/ASocket.cpp\
			srcs/socketInit/ListeningSocket.cpp\
			srcs/serverHandling/Server.cpp\
			srcs/configFile/1-parseInput/Block.cpp\
			srcs/configFile/1-parseInput/Recup.cpp\
			srcs/configFile/1-parseInput/utils.cpp\
			srcs/configFile/2-assignValues/ConfigData.cpp\
			
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