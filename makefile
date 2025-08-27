CC := gcc
CXXFLAGS := -std=c++17 -Wall -Wextra -pedantic -fPIC -g -O0 -fsanitize=thread
LDFLAGS := -pthread -lstdc++
SHARED_FLAGS := -shared

# Динамическая библиотека logger
LIB_SRCS := logger.cpp
LIB_NAME := liblogger.so

# Основное приложение
APP_SRCS := app.cpp main.cpp
APP_NAME := logapp

# Тесты
TEST_BASIC_SRCS := test_basic.cpp app.cpp
TEST_BASIC_NAME := test_basic

TEST_CONCURRENT_SRCS := test_concurrent.cpp app.cpp
TEST_CONCURRENT_NAME := test_concurrent

TEST_DEADLOCK_SRCS := test_deadlock.cpp app.cpp
TEST_DEADLOCK_NAME := test_deadlock

TEST_STRESS_SRCS := test_stress.cpp app.cpp
TEST_STRESS_NAME := test_stress

.PHONY: all lib app test clean

all: lib app

# Сборка динамической библиотеки
lib: $(LIB_NAME)

$(LIB_NAME): $(LIB_SRCS)
	$(CC) $(CXXFLAGS) $(SHARED_FLAGS) -o $@ $^ $(LDFLAGS)

# Сборка основного приложения
app: $(APP_NAME)

$(APP_NAME): $(APP_SRCS) $(LIB_NAME)
	$(CC) $(CXXFLAGS) -o $@ $(APP_SRCS) -L. -llogger $(LDFLAGS) -Wl,-rpath,.

# Сборка тестов
test: $(TEST_BASIC_NAME) $(TEST_CONCURRENT_NAME) $(TEST_DEADLOCK_NAME) $(TEST_STRESS_NAME) 

$(TEST_BASIC_NAME): $(TEST_BASIC_SRCS) $(LIB_NAME)
	$(CC) $(CXXFLAGS) -o $@ $(TEST_BASIC_SRCS) -L. -llogger $(LDFLAGS) -Wl,-rpath,.

$(TEST_CONCURRENT_NAME): $(TEST_CONCURRENT_SRCS) $(LIB_NAME)
	$(CC) $(CXXFLAGS) -o $@ $(TEST_CONCURRENT_SRCS) -L. -llogger $(LDFLAGS) -Wl,-rpath,.

$(TEST_DEADLOCK_NAME): $(TEST_DEADLOCK_SRCS) $(LIB_NAME)
	$(CC) $(CXXFLAGS) -o $@ $(TEST_DEADLOCK_SRCS) -L. -llogger $(LDFLAGS) -Wl,-rpath,.

$(TEST_STRESS_NAME): $(TEST_STRESS_SRCS) $(LIB_NAME)
	$(CC) $(CXXFLAGS) -o $@ $(TEST_STRESS_SRCS) -L. -llogger $(LDFLAGS) -Wl,-rpath,.


clean:
	rm -f $(LIB_NAME) $(APP_NAME) $(TEST_BASIC_NAME) $(TEST_CONCURRENT_NAME) $(TEST_DEADLOCK_NAME) $(TEST_STRESS_NAME) 

# Правило для запуска тестов (после их сборки)
run-tests: test
	@echo "Running basic test..."
	./$(TEST_BASIC_NAME)
	@echo "Running concurrent test..."
	./$(TEST_CONCURRENT_NAME)
	@echo "Running deadlock test..."
	./$(TEST_DEADLOCK_NAME)
	@echo "Running stress test..."
	./$(TEST_STRESS_NAME)
	@echo "All tests completed!"