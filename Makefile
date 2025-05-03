# Properties

NAME				:= complexity

# Commands

CXXFLAGS			:= -std=c++17 -Wall -Wextra -O3

# Sources

override SRCS		:=							\
				main.cpp						\
				utils.cpp						\
				print.cpp						\
				args.cpp						\
				executor.cpp					\
				arguments_generator.cpp			\

override HEADERS	:=							\
				complexity.hpp					\
				executor.hpp					\
				arguments_generator.hpp			\
				thread_safe_random.hpp			\

override HEADERS	:= $(addprefix src/,$(HEADERS))

override OBJS		:= $(addprefix obj/, $(SRCS:.cpp=.o))

override OBJDIRS	:= $(sort $(dir $(OBJS)))

# Rules

all:		$(NAME)

obj/%.o:	src/%.cpp $(HEADERS)
			$(CXX) $(CXXFLAGS) -c $< -o $@ -Iincludes

$(OBJS):	| $(OBJDIRS)

$(OBJDIRS):
			mkdir -p $@

$(NAME):	$(OBJS)
			$(CXX) $(CXXFLAGS) -o $@ $(OBJS)

clean:
			rm -rf obj

fclean:		clean
			rm -f $(NAME)

re:			fclean all

.PHONY:		all clean fclean re
