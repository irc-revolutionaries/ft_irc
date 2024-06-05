CXX		=	c++
FLAGS	=	-Wall -Wextra -Werror -std=c++98
SRCS	=	main.cpp \
			Channel.cpp
			Client.cpp \
			Command.cpp \
			Server.cpp
OBJS	=	$(SRCS:.cpp=.o)
HEADER	=	essential.hpp \
			Channel.hpp \
			Client.hpp \
			Command.hpp \
			Server.hpp
NAME	=	ircserv

all : $(NAME)

$(NAME) : $(OBJS) $(HEADER)
	$(CXX) $(FLAGS) $(OBJS) -o $(NAME)

%.o : %.cpp $(HEADER)
	$(CXX) $(FLAGS) -c $< -o $@

clean : 
	rm -rf $(OBJS)

fclean : 
	make clean
	rm -rf $(NAME)

re :
	make fclean
	make all

.PHONY : all clean fclean re
