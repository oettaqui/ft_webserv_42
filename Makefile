# NAME = Webserv
# CXX = c++
# CPPFLAGS = -Wall -Wextra -Werror -std=c++98 -fsanitize=address -g3 
# SRCS =	srcs/main.cpp srcs/Parse_configfile/ConfigParser.cpp srcs/Parse_configfile/Location.cpp\
# 		srcs/Parse_configfile/Server.cpp srcs/build_server/WebServer.cpp\
# 		srcs/pars_request/ParsRequest.cpp \
# 		srcs/post_handler/PostHandler.cpp srcs/get_handler/GetHandler.cpp srcs/delete_handler/DeleteHandler.cpp\
# 		srcs/CGI/CGI.cpp
# OBJS = ${SRCS:.cpp=.o}
# RM = rm -f

# all : ${NAME}

# ${NAME} : ${OBJS}
# 	${CXX} ${CPPFLAGS} ${OBJS} -o ${NAME}

# clean :
# 	${RM} ${OBJS}

# fclean : clean
# 	${RM} ${NAME}

# re : fclean all

# .PHONY : all clean fclean re



NAME		= Webserv
CXX			= c++
CPPFLAGS	= -Wall -Wextra -Werror -std=c++98 -fsanitize=address -g3

# Source files organized by category
MAIN_SRCS	= srcs/main.cpp
CONFIG_SRCS	= srcs/Parse_configfile/ConfigParser.cpp \
			  srcs/Parse_configfile/Location.cpp \
			  srcs/Parse_configfile/Server.cpp
SERVER_SRCS	= srcs/build_server/WebServer.cpp
REQUEST_SRCS= srcs/pars_request/ParsRequest.cpp
HANDLER_SRCS= srcs/post_handler/PostHandler.cpp \
			  srcs/get_handler/GetHandler.cpp \
			  srcs/delete_handler/DeleteHandler.cpp
CGI_SRCS	= srcs/CGI/CGI.cpp

SRCS		= $(MAIN_SRCS) $(CONFIG_SRCS) $(SERVER_SRCS) $(REQUEST_SRCS) $(HANDLER_SRCS) $(CGI_SRCS)
OBJS		= ${SRCS:.cpp=.o}
OBJS_DIR	= objs/
OBJS_FULL	= $(addprefix $(OBJS_DIR), $(OBJS))

RM			= rm -f
MKDIR		= mkdir -p

# ================================= COLORS ==================================== #

RED			= \033[1;31m
GREEN		= \033[1;32m
YELLOW		= \033[1;33m
BLUE		= \033[1;34m
MAGENTA		= \033[1;35m
CYAN		= \033[1;36m
WHITE		= \033[1;37m
RESET		= \033[0m
BOLD		= \033[1m

# Progress bar variables
TOTAL_FILES	= $(words $(SRCS))
COMPILED	= 0

# ================================== RULES =================================== #

all: header $(NAME) footer

.SILENT:

header:
	@echo "$(CYAN)"
	@echo "╔═══════════════════════════════════════════════════════════════════════════════╗"
	@echo "║                              🌐 WEBSERV PROJECT 🌐                            ║"
	@echo "║                           Building HTTP Server in C++                        ║"
	@echo "╚═══════════════════════════════════════════════════════════════════════════════╝"
	@echo "$(RESET)"

$(OBJS_DIR):
	@$(MKDIR) $(OBJS_DIR)
	@$(MKDIR) $(OBJS_DIR)srcs/Parse_configfile
	@$(MKDIR) $(OBJS_DIR)srcs/build_server
	@$(MKDIR) $(OBJS_DIR)srcs/pars_request
	@$(MKDIR) $(OBJS_DIR)srcs/post_handler
	@$(MKDIR) $(OBJS_DIR)srcs/get_handler
	@$(MKDIR) $(OBJS_DIR)srcs/delete_handler
	@$(MKDIR) $(OBJS_DIR)srcs/CGI

$(OBJS_DIR)%.o: %.cpp | $(OBJS_DIR)
	@$(eval COMPILED=$(shell echo $$(($(COMPILED)+1))))
	@$(eval PERCENT=$(shell echo $$(($(COMPILED)*100/$(TOTAL_FILES)))))
	@printf "$(BLUE)┌─ Compiling [$(YELLOW)%3d%%$(BLUE)] $(GREEN)%-50s$(BLUE) ┐$(RESET)\r" $(PERCENT) "$<"
	@$(CXX) $(CPPFLAGS) -c $< -o $@
	@printf "\033[2K"

$(NAME): $(OBJS_FULL)
	@echo "$(MAGENTA)"
	@echo "┌────────────────────────────────────────────────────────────────────────────────┐"
	@echo "│                           🔗 LINKING OBJECT FILES 🔗                           │"
	@echo "└────────────────────────────────────────────────────────────────────────────────┘"
	@echo "$(RESET)"
	@$(CXX) $(CPPFLAGS) $(OBJS_FULL) -o $(NAME)
	@echo "$(GREEN)$(BOLD)"
	@echo "╔════════════════════════════════════════════════════════════════════════════════╗"
	@echo "║                          ✅ COMPILATION SUCCESSFUL ✅                          ║"
	@echo "║                             $(NAME) is ready to serve!                            ║"
	@echo "╚════════════════════════════════════════════════════════════════════════════════╝"
	@echo "$(RESET)"

clean:
	@echo "$(YELLOW)🧹 Cleaning object files...$(RESET)"
	@$(RM) -rf $(OBJS_DIR)
	@echo "$(RED)🗑️  Object files deleted$(RESET)"

fclean: clean
	@echo "$(YELLOW)🧹 Deep cleaning...$(RESET)"
	@$(RM) $(NAME)
	@echo "$(RED)🗑️  $(NAME) executable deleted$(RESET)"

re: fclean all

footer:
	@echo "$(CYAN)"
	@echo "╔═══════════════════════════════════════════════════════════════════════════════╗"
	@echo "║                          🚀 Ready to launch server! 🚀                       ║"
	@echo "║                          Usage: ./$(NAME) [config_file]                           ║"
	@echo "╚═══════════════════════════════════════════════════════════════════════════════╝"
	@echo "$(RESET)"

# Debug rule to show variables
debug:
	@echo "$(BLUE)Debug Information:$(RESET)"
	@echo "NAME: $(NAME)"
	@echo "SRCS: $(SRCS)"
	@echo "OBJS: $(OBJS_FULL)"
	@echo "TOTAL_FILES: $(TOTAL_FILES)"

# Help rule
help:
	@echo "$(CYAN)$(BOLD)Available targets:$(RESET)"
	@echo "  $(GREEN)all$(RESET)     - Build the $(NAME) executable"
	@echo "  $(GREEN)clean$(RESET)   - Remove object files"
	@echo "  $(GREEN)fclean$(RESET)  - Remove object files and executable"
	@echo "  $(GREEN)re$(RESET)      - Rebuild everything from scratch"
	@echo "  $(GREEN)debug$(RESET)   - Show debug information"
	@echo "  $(GREEN)help$(RESET)    - Show this help message"

.PHONY: all clean fclean re debug help header footer