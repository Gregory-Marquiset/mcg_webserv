NAME := request_test

CXX := c++

CXXFLAGS += -Wall -Wextra -Werror -std=c++98 -g3

SRCS :=		RequestParser.cpp\
			tester.cpp

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
