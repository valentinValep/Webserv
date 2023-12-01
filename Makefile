NAME := webserv
TEST_NAME := unit_tests

CXX := c++

CXXFLAGS := -Wall -Werror -Wextra -std=c++98 -MMD -g3

MAKE := make

SOURCES_DIR := src/
BUILD_DIR := build/
INCLUDES_DIR := includes/
LIBS_DIR := lib/

OBJ := $(BUILD_DIR)Server.o \
	$(BUILD_DIR)ServerManager.o \
	$(BUILD_DIR)ClientRequest.o \
	$(BUILD_DIR)Route.o \
	$(BUILD_DIR)utils.o \
	$(BUILD_DIR)Body.o \
	$(BUILD_DIR)ServerReactor.o \
	$(BUILD_DIR)EventHandler.o \
	$(BUILD_DIR)AcceptHandler.o \
	$(BUILD_DIR)ProcessHandler.o \
	$(BUILD_DIR)ProcessState.o \
	$(BUILD_DIR)ReadState.o \
	$(BUILD_DIR)ResponseBuildState.o \
	$(BUILD_DIR)ResponseBuildingStrategy.o \
	$(BUILD_DIR)ErrorStrategy.o \
	$(BUILD_DIR)RedirectStrategy.o \
	$(BUILD_DIR)CgiStrategy.o \
	$(BUILD_DIR)UploadStrategy.o \
	$(BUILD_DIR)DeleteStrategy.o \
	$(BUILD_DIR)ResponseSendState.o \
	$(BUILD_DIR)GetStrategyCreator.o \
	$(BUILD_DIR)GetAutoIndexStrategy.o \
	$(BUILD_DIR)GetFileStrategy.o \
	$(BUILD_DIR)Response.o \
	$(BUILD_DIR)FileReader.o \
	$(BUILD_DIR)CgiChildHandler.o \

MAIN_OBJ := $(BUILD_DIR)main.o

TEST_OBJ := $(BUILD_DIR)tests/test.o

INCLUDES := -I$(INCLUDES_DIR) \
			-I$(LIBS_DIR)fileParser/includes

LIBS := $(LIBS_DIR)fileParser/libfp.a

LIBS_FLAGS := -L$(LIBS_DIR)fileParser -lfp

DEPS := ${OBJ:.o=.d} ${MAIN_OBJ:.o=.d} ${TEST_OBJ:.o=.d}

#############################################################################

$(NAME): $(OBJ) $(MAIN_OBJ) $(LIBS)
	$(CXX) $(OBJ) $(MAIN_OBJ) -o $(NAME) $(LIBS_FLAGS)

$(TEST_NAME): $(OBJ) $(TEST_OBJ) $(LIBS)
	$(CXX) $(OBJ) $(TEST_OBJ) -o $(TEST_NAME) $(LIBS_FLAGS)

$(LIBS_DIR)fileParser/libfp.a:
	make -C $(LIBS_DIR)fileParser

$(LIBS_DIR)fileParser/:
	git submodule update --init --recursive

test: $(TEST_NAME)
	./$(TEST_NAME)

$(BUILD_DIR):
	mkdir $(BUILD_DIR)
	mkdir $(BUILD_DIR)tests

$(BUILD_DIR)%.o: $(SOURCES_DIR)%.cpp | $(BUILD_DIR) $(LIBS_DIR)fileParser/
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

all: $(NAME)

clean:
	rm -rf $(BUILD_DIR)
	if [ -d $(LIBS_DIR)fileParser/ ] && [ -f $(LIBS_DIR)fileParser/Makefile ]; then make -C $(LIBS_DIR)fileParser clean; else rm -rf $(LIBS_DIR)fileParser; fi

fclean: clean
	rm -f $(NAME) $(TEST_NAME)
	if [ -d $(LIBS_DIR)fileParser/ ] && [ -f $(LIBS_DIR)fileParser/Makefile ]; then make -C $(LIBS_DIR)fileParser fclean; else rm -rf $(LIBS_DIR)fileParser; fi

re: fclean
	$(MAKE) all

.PHONY: all clean fclean re test

-include $(DEPS)