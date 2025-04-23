NAME = Webserv
CXX = c++
CPPFLAGS = -Wall -Wextra -Werror -std=c++98 -fsanitize=address -g3 
SRCS = main.cpp Server.cpp ParsRequest/ParsRequest.cpp
OBJS = ${SRCS:.cpp=.o}
RM = rm -f

all : ${NAME}

${NAME} : ${OBJS}
	${CXX} ${CPPFLAGS} ${OBJS} -o ${NAME}

clean :
	${RM} ${OBJS}

fclean : clean
	${RM} ${NAME}

re : fclean all

.PHONY : all clean fclean re