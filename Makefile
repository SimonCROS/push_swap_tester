# Properties

NAME				= complexity

# Commands

override CPPC		:= clang++
override CPPFLAGS	:= -std=c++17 -Wall -Wextra
override RM			:= rm

# Sources

override SRCS		:=							\
				main.cpp						\
				exec.cpp						\
				args.cpp						\
				print.cpp						\
				utils.cpp						\

override HEADERS	:=							\
				complexity.hpp

override OBJS		:= $(addprefix obj/, $(SRCS:.cpp=.o))

# Rules

all:		$(NAME)

obj/%.o:	src/%.cpp $(addprefix includes/,$(HEADERS))
			$(CPPC) $(CPPFLAGS) -c $< -o $@ -Iincludes

$(NAME):	$(OBJS)
			$(CPPC) $(CPPFLAGS) -o $@ $(OBJS)

clean:
			$(RM) -r obj

fclean:		clean
			$(RM) $(NAME)

re:			fclean all

.PHONY:		all clean fclean re
