CC = gcc
CFLAGS = -Wall -Wextra -pedantic -g 
LDFLAGS = -pg --coverage
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

.PHONY: backend backend_test clean

$(BUILD_DIR)/%.o: %.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -c $< -o $@

backend: $(BACKEND_O) $(VENDOR_O)

$(BUILD_DIR)/backend_test: $(BUILD_DIR)/test/backend.o $(BACKEND_O) $(VENDOR_O)
	$(CC) $(LDFLAGS) $^ -o $@ $(LDLIBS) $(CUNIT_LIBS)

backend_test: $(BUILD_DIR)/backend_test
	./$<


clean:
	rm -rf $(BUILD_DIR)
