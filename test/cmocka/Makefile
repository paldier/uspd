CC = gcc
USPD_LIB_DIR ?= $(shell dirname $(PWD))
USPD_LIB = -luspd -L$(USPD_LIB_DIR)
CMOCKA_LIB = -l cmocka
LIBS = $(USPD_LIB) $(CMOCKA_LIB) -luci -pthread -lubus -lubox -ljson-c -lblobmsg_json -lbbfdm
CFLAGS = -g -Wall -I../..
LDFLAGS = $(LIBS) -Wl,-rpath=$(USPD_LIB_DIR) -I$(USPD_LIB_DIR)
UNIT_TESTS = unit_test_uspd

VALGRIND = valgrind --leak-check=full --show-reachable=no \
           --show-leak-kinds=all --errors-for-leak-kinds=all \
           --error-exitcode=1 --track-origins=yes

unit_test_uspd:  unit_test_uspd.o
	$(CC) -o $@ $^ $(LDFLAGS)

unit-test: $(UNIT_TESTS)
	$(foreach testprog, $(UNIT_TESTS), sudo $(VALGRIND) ./$(testprog);)

.PHONY: clean
clean:
	rm $(UNIT_TESTS) *.o -fv

