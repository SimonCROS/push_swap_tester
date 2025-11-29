# Properties

NAME				:= complexity

# Commands

CXXFLAGS			:= -std=c++17 -Wall -Wextra -O3

# Sources

override SRCS		:=							\
				main.cpp						\
				worker.cpp						\
				utils.cpp						\
				print.cpp						\
				args.cpp						\
				executor.cpp					\
				arguments_generator.cpp			\

override OBJS		:= $(addprefix build/, $(SRCS:.cpp=.o))

override DEPS		:= $(OBJS:.o=.d)

override OBJDIRS	:= $(sort $(dir $(OBJS)))

# Rules

all:		$(NAME)

build/%.o:	src/%.cpp
			$(CXX) $(CXXFLAGS) -c $< -o $@ -MMD -MP

$(OBJS):	| $(OBJDIRS)

$(OBJDIRS):
			mkdir -p $@

$(NAME):	$(OBJS)
			$(CXX) $(CXXFLAGS) -o $@ $(OBJS)

clean:
			rm -rf build

fclean:		clean
			rm -f $(NAME)

re:			fclean all

.PHONY:		all clean fclean re

-include $(DEPS)
