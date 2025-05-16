NAME = Webserv
CXX = c++
CPPFLAGS = -Wall -Wextra -Werror -std=c++98 -fsanitize=address -g3 
SRCS =	srcs/main.cpp srcs/Parse_configfile/ConfigParser.cpp srcs/Parse_configfile/Location.cpp\
		srcs/Parse_configfile/Server.cpp srcs/build_server/WebServer.cpp\
		srcs/pars_request/ParsRequest.cpp \
		srcs/post_handler/PostHandler.cpp
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