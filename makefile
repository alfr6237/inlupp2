CC = gcc
CFLAGS = -Wall -Wextra -pedantic -g --coverage
LDFLAGS = --coverage
LDLIBS = -lm
CUNIT_LIBS = -lcunit

BUILD_DIR = build
BACKEND_DIR = src/backend
FRONTEND_DIR = src/frontend

HT_O = $(addprefix $(BUILD_DIR)/,vendor/hash_table.o vendor/hash_table_iterator.o)
LL_O = $(addprefix $(BUILD_DIR)/,vendor/linked_list.o vendor/linked_list_iterator.o)
UTILS_O = $(BUILD_DIR)/vendor/utils.o

VENDOR_O = $(HT_O) $(LL_O) $(UTILS_O)

BACKEND_SRC = $(wildcard $(BACKEND_DIR)/*.c)
BACKEND_O = $(patsubst %.c,$(BUILD_DIR)/%.o,$(BACKEND_SRC))

FRONTEND_SRC = $(wildcard $(FRONTEND_DIR)/*.c)
FRONTEND_O = $(patsubst %.c,$(BUILD_DIR)/%.o,$(FRONTEND_SRC))

VALGRIND = valgrind --leak-check=full --show-leak-kinds=all --error-exitcode=1

.PHONY: backend_test frontend_run frontend_test test coverage
.PHONY: valgrind_backend valgrind_frontend valgrind clean

$(BUILD_DIR)/%.o: %.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/backend_test: $(BUILD_DIR)/test/backend.o $(BACKEND_O) $(VENDOR_O)
	$(CC) $(LDFLAGS) $^ -o $@ $(LDLIBS) $(CUNIT_LIBS)

$(BUILD_DIR)/frontend: $(FRONTEND_O) $(VENDOR_O) $(BACKEND_O)
	$(CC) $(LDFLAGS) $^ -o $@ $(LDLIBS) $(CUNIT_LIBS)

backend_test: $(BUILD_DIR)/backend_test
	./$<

frontend_run: $(BUILD_DIR)/frontend
	./$<

frontend_test: $(BUILD_DIR)/frontend
	./$< < test.txt

test: backend_test frontend_test


coverage: $(BUILD_DIR)/backend_test $(BUILD_DIR)/frontend
	$(MAKE) test
	rm -rf $(BUILD_DIR)/coverage
	mkdir -p $(BUILD_DIR)/coverage
	@for obj in $(BACKEND_O) $(FRONTEND_O); do \
		src=$${obj#$(BUILD_DIR)/}; \
		src=$${src%.o}.c; \
		gcov -b -c -o "$$obj" "$$src" || exit 1; \
	done
	mv *.gcov $(BUILD_DIR)/coverage/

valgrind_backend: $(BUILD_DIR)/backend_test
	$(VALGRIND) ./$<

valgrind_frontend: $(BUILD_DIR)/frontend
	$(VALGRIND) ./$< < test.txt

valgrind: valgrind_backend valgrind_frontend

clean:
	rm -rf $(BUILD_DIR)
