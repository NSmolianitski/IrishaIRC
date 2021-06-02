NAME		= ircserv

SRCS		= 	main.cpp AConnection.cpp Channel.cpp Irisha.config.cpp Irisha.cpp Irisha.irc.cpp Irisha.replies.cpp \
				Irisha.users.cpp Irisha.utils.cpp parser.cpp Server.cpp User.cpp utils.cpp
OBJS		= $(SRCS:.cpp=.o)

CC			= clang++
FLAGS		= -std=c++11

.cpp.o:
			clang++ $(FLAGS) -c $< -o ${<:.cpp=.o}

all:		$(NAME)

$(NAME):	$(OBJS)
			$(CC) $(FLAGS) $(OBJS) -o $(NAME)

clean:
			rm -f $(OBJS)

fclean:		clean
			rm -f $(NAME)

re:			fclean all

.PHONY:		all clean fclean re