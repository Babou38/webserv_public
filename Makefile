# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: lechaps <lechaps@student.42.fr>            +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/04/22 18:57:28 by lechaps           #+#    #+#              #
#    Updated: 2025/07/18 10:34:52 by lechaps          ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME = webserv
FLAGS = -Wall -Wextra -Werror -std=c++98
INC = -Iincludes/
SRC_DIR = source/
OBJ_DIR = objs/

SRC_NAME =	main.cpp\
			dispatch.cpp\
			parsing/ConfigParser.cpp\
			class/LocationConfig.cpp\
			class/ServerConfig.cpp\
			class/HttpRequest.cpp\
			class/HttpResponse.cpp\
			class/config.cpp\
			class/NetworkServer.cpp\
			method/post.cpp\
			method/get.cpp\
			method/delete.cpp\
			method/cgi.cpp\
			method/redirect.cpp\
			utiles.cpp\

OBJ_NAME = $(SRC_NAME:.cpp=.o)
OBJ = $(addprefix $(OBJ_DIR),$(OBJ_NAME))

$(OBJ_DIR)%.o: $(SRC_DIR)%.cpp includes/Webserv.hpp Makefile
	@mkdir -p $(OBJ_DIR)
	@mkdir -p $(OBJ_DIR)/class
	@mkdir -p $(OBJ_DIR)/method
	@mkdir -p $(OBJ_DIR)/parsing

	@echo "##### Creating" [ $@ ] " #####"
	@c++ $(FLAGS) -o $@ -c $< $(INC)

$(NAME): $(OBJ) 
	@c++ -o $(NAME) $(OBJ)
	@echo "\033[0;32m##### $(NAME) created #####\033[0m"

all: $(NAME)

clean:
	@rm -fr $(CHECK_OBJ) $(OBJ)
	@rm -fr $(OBJ_DIR)
	@echo "\033[0;33m##### Removed object files #####\033[0m"

fclean: clean
	@rm -f $(NAME)
	@echo "\033[0;33m##### Removed binary files #####\033[0m"

re: fclean all

.PHONY: all clean fclean re%   