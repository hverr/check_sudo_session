CC=gcc
LD=gcc
LDFLAGS=
CFLAGS=-Wall

TARGET=check_sudo_session
SOURCES=check_sudo_session.c\
	process.c\
	options.c
OBJECTS=$(SOURCES:%.c=%.o)

%.c: %.h

%.o: %.c
	$(CC) -c -o $@ $< $(CFLAGS)

$(TARGET): $(OBJECTS)
	$(LD) -o $@ $^ $(LDFLAGS)

.PHONY: all clean
all: $(TARGET)

clean:
	rm -f $(TARGET)
	rm -f $(OBJECTS)

