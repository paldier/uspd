PROG = uspd
OBJS =common.o get.o set.o operate.o add_delete.o dummy.o usp.o

PROG_CFLAGS = $(CFLAGS) -fstrict-aliasing -Wall -Wextra -Werror -DDM_USE_LIBUBUS -Wformat -Wformat-signedness -Wsign-conversion
PROG_LDFLAGS = $(LDFLAGS)
PROG_LDFLAGS += -luci -lubus -lubox -ljson-c -lblobmsg_json -lbbfdm

%.o: %.c
	$(CC) $(PROG_CFLAGS) $(FPIC) -c -o $@ $<

all: uspd

uspd: $(OBJS)
	$(CC) -o $@ $^ $(PROG_LDFLAGS)

clean:
	rm -f *.o $(PROG)
