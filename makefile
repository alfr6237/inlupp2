CC = gcc
CFLAGS = -Wall -Wextra -pedantic -g 
LDFLAGS = --coverage
LDLIBS = -lm
CUNIT_LIBS = -lcunit

BUILD_DIR = build
BACKEND_DIR = src/backend

HT_O = $(addprefix $(BUILD_DIR)/,vendor/hash_table.o vendor/hash_table_iterator.o)
LL_O = $(addprefix $(BUILD_DIR)/,vendor/linked_list.o vendor/linked_list_iterator.o)
UTILS_O  = $(BUILD_DIR)/vendor/utils.o

VENDOR_O = $(HT_O) $(LL_O) $(UTILS_O)

BACKEND_SRC = $(wildcard $(BACKEND_DIR)/*.c)
BACKEND_O = $(patsubst %.c,$(BUILD_DIR)/%.o,$(BACKEND_SRC))


FRONTEND_SRC = $(wildcard $(FRONTEND_DIR)/*.c)
FRONTEND_O = $(patsubst %.c,$(BUILD_DIR)/%.o,$(FRONTEND_SRC))


.PHONY: backend backend_test clean frontend

$(BUILD_DIR)/%.o: %.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -c $< -o $@



$(BUILD_DIR)/backend_test: $(BUILD_DIR)/test/backend.o $(BACKEND_O) $(VENDOR_O)
	$(CC) $(LDFLAGS) $^ -o $@ $(LDLIBS) $(CUNIT_LIBS)


backend_test: $(BUILD_DIR)/backend_test
	./$<


$(BUILD_DIR)/frontend: $(BUILD_DIR)/src/frontend/tui.o $(FRONTEND_O) $(VENDOR_O) $(BACKEND_O)
	$(CC) $(LDFLAGS) $^ -o $@ $(LDLIBS) $(CUNIT_LIBS)

frontend_run: $(BUILD_DIR)/frontend
	./$<




valgrind_backend: $(BUILD_DIR)/backend_test
	valgrind ./$<

clean:
	rm -rf $(BUILD_DIR)
