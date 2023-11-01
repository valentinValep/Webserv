NAME := webserv
TEST_NAME := tests

CXX := c++

CXXFLAGS := -Wall -Werror -Wextra -std=c++98 -MMD -g3

SOURCES_DIR := src/
BUILD_DIR := build/
INCLUDES_DIR := includes/

OBJ := $(BUILD_DIR)Server.o $(BUILD_DIR)ServerResponse.o $(BUILD_DIR)ClientRequest.o $(MAIN_OBJ) $(TEST_OBJ)

MAIN_OBJ := $(BUILD_DIR)main.o

TEST_OBJ := $(BUILD_DIR)tests/test.o

INCLUDES := -I$(INCLUDES_DIR)

DEPS := ${OBJ:.o=.d} ${MAIN_OBJ:.o=.d} ${TEST_OBJ:.o=.d}

#############################################################################

$(NAME): $(OBJ) $(MAIN_OBJ)
	$(CXX) $(OBJ) $(MAIN_OBJ) -o $(NAME)

$(TEST_NAME): $(OBJ) $(TEST_OBJ)
	$(CXX) $(OBJ) $(TEST_OBJ) -o $(TEST_NAME)

test: $(TEST_NAME)
	./$(TEST_NAME)

$(BUILD_DIR):
	mkdir $(BUILD_DIR)

$(BUILD_DIR)%.o: $(SOURCES_DIR)%.cpp | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

all: $(NAME)

clean:
	rm -rf $(BUILD_DIR)

fclean: clean
	rm -f $(NAME) $(TEST_NAME)

re: fclean all

.PHONY: all clean fclean re test

-include $(DEPS)