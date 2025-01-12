# Properties

NAME				:= complexity

# Commands

override CPPC		:= clang++
override CPPFLAGS	:= -std=c++20 -Wall -Wextra

# Sources

override SRCS		:=							\
				main.cpp						\
				executor.cpp					\
				arguments_generator.cpp			\

override HEADERS	:=							\
				complexity.hpp					\
				arguments_generator.hpp			\
				executor.hpp					\

override HEADERS	:= $(addprefix src/,$(HEADERS))

override OBJS		:= $(addprefix obj/, $(SRCS:.cpp=.o))

override OBJDIRS	:= $(sort $(dir $(OBJS)))

# Rules

all:		$(NAME)

obj/%.o:	src/%.cpp $(HEADERS)
			$(CPPC) $(CPPFLAGS) -c $< -o $@ -Iincludes

$(OBJS):	| $(OBJDIRS)

$(OBJDIRS):
			mkdir -p $@

$(NAME):	$(OBJS)
			$(CPPC) $(CPPFLAGS) -o $@ $(OBJS)

clean:
			rm -rf obj

fclean:		clean
			rm -f $(NAME)

re:			fclean all

.PHONY:		all clean fclean re
