PROG = uspd
OBJS =common.o get.o set.o operate.o add_delete.o usp.o
CP=cp -f

PROG_CFLAGS = $(CFLAGS) \
	      -fstrict-aliasing \
	      -Wall -Wextra -Werror \
	      -Wformat \
	      -Wformat-signedness

PROG_LDFLAGS = $(LDFLAGS)
PROG_LDFLAGS += -luci -lubus -lubox -ljson-c -lblobmsg_json -lbbfdm
GCOV = gcov
CODECOVERAGE_SRC = usp.c


%.o: %.c
	$(CC) $(PROG_CFLAGS) -c -o $@ $<

all: ${PROG}

${PROG}: $(OBJS)
	$(CC) $(PROG_CFLAGS) -o $@ $^ $(PROG_LDFLAGS)
	$(CP) ${PROG} ../${PROG}

test: PROG_CFLAGS += -fPIC
test: ${OBJS}
	${CC} $(PROG_CFLAGS) -shared -o libuspd.so ${OBJS} $(PROG_LDFLAGS)
	$(CP) libuspd.so ../libuspd.so

unit-test: coverage
	make -C ../test/cmocka unit-test USPD_LIB_DIR=$(PWD)

coverage: CFLAGS  += -g -O0 -fprofile-arcs -ftest-coverage
coverage: LDFLAGS += --coverage
coverage: clean_objs test ${PROG}
	$(foreach testprog, $(CODECOVERAGE_SRC), $(GCOV) $(testprog);)

clean_objs:
	rm -f *.o
clean:
	rm -f *.o libuspd.so $(PROG)
	rm -f *.xml *.html
	find -name '*.gcda' -exec rm {} -fv \;
	find -name '*.gcno' -exec rm {} -fv \;
	find -name '*.gcov' -exec rm {} -fv \;
