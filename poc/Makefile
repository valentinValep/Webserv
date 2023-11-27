NAME := webserv_poc

CXX := c++

CXXFLAGS := -Wall -Werror -Wextra -std=c++98 -MMD -g3 -fsanitize=address,leak,undefined

MAKE := make

OBJ := main.o \

INCLUDES := -I. \

LIBS :=

LIBS_FLAGS :=

DEPS := ${OBJ:.o=.d} ${MAIN_OBJ:.o=.d} ${TEST_OBJ:.o=.d}

#############################################################################

$(NAME): $(OBJ)
	$(CXX) $(OBJ) -fsanitize=address,leak,undefined -o $(NAME) $(LIBS_FLAGS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

all: $(NAME)

clean:
	rm -rf $(OBJ)

fclean: clean
	rm -f $(NAME)

re: fclean
	$(MAKE) all

.PHONY: all clean fclean re test

-include $(DEPS)