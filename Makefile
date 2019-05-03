PROG = uspd
OBJS =common.o get.o set.o operate.o dummy.o usp.o

PROG_CFLAGS = $(CFLAGS) -fstrict-aliasing -Wall -Wextra -DDM_USE_LIBUBUS
PROG_LDFLAGS = $(LDFLAGS)
PROG_LDFLAGS += -luci -lubus -lubox -ljson-c -lblobmsg_json -ldatamodel

%.o: %.c
	$(CC) $(PROG_CFLAGS) $(FPIC) -c -o $@ $<

all: uspd

uspd: $(OBJS)
	$(CC) $(PROG_LDFLAGS) -o $@ $^

clean:
	rm -f *.o $(PROG)
