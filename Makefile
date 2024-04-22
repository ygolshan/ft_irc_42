# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: ygolshan <ygolshan@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2024/03/26 12:50:56 by ygolshan          #+#    #+#              #
#    Updated: 2024/03/26 19:42:31 by ygolshan         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME		:= ircserv
CXX			:=	c++
CXXFLAGS	:= -std=c++98 -Wall -Wextra -Werror
LDFLAGS		:=

SRCDIR		:=	srcs
INCDIR		:=	includes

SRCS		:=	$(SRCDIR)/Channel.cpp\
				$(SRCDIR)/Client.cpp \
				$(SRCDIR)/Invitation.cpp \
				$(SRCDIR)/Kick.cpp\
				$(SRCDIR)/Mode.cpp\
				$(SRCDIR)/Command.cpp\
				$(SRCDIR)/main.cpp\
				$(SRCDIR)/Server.cpp\

INCS		:=	$(INCDIR)/Channel.hpp\
				$(INCDIR)/Client.hpp\
				$(INCDIR)/Command.hpp\
				$(INCDIR)/Define.hpp\
				$(INCDIR)/Server.hpp\

OBJS		:=	$(SRCS:.cpp=.o)



.SUFFIXES : .cpp .o
.cpp.o :
	$(CXX) $(CXXFLAGS) -c $< -o $@

all	: $(NAME)

$(NAME)	:	$(OBJS)
	$(CXX) $(LDFLAGS) $(OBJS) -o $@

clean :
	rm -f $(OBJS)

fclean :
	make clean
	rm -f $(NAME)

re :
	make fclean
	make $(NAME)

.PHONY : all clean fclean re