# Properties

NAME				:= complexity

# Commands

override CPPC		:= clang++
override CPPFLAGS	:= -std=c++17 -Wall -Wextra

# Sources

LANG				:= fr_FR

override SRCS		:=							\
				main.cpp						\
				exec.cpp						\
				args.cpp						\
				print.cpp						\
				utils.cpp						\
				lang/$(LANG).cpp				\

override HEADERS	:=							\
				complexity.hpp					\

override HEADERS	:= $(addprefix includes/,$(HEADERS))

override OBJS		:= $(addprefix obj/, $(SRCS:.cpp=.o))

override OBJDIRS	:= $(sort $(dir $(OBJS)))

# Rules

all:		$(NAME)

fr:
			touch src/lang/fr_FR.cpp
			$(MAKE) LANG="fr_FR"

en:
			touch src/lang/en_GB.cpp
			$(MAKE) LANG="en_GB"

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
