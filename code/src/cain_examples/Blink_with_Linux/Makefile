#CC  = arm-linux-gcc
CC=arm-linux-gcc

all: test_gpio

DEBUG = -O3 -g0

COMMON_SOURCES =   

H_SOURCES = board.h

INCLUDE = -I. -Iinclude/

WARNINGS= -Wcast-align -Wpacked -Wpadded -Wall

CCFLAGS = ${INCLUDE} ${DEBUG} ${WARNINGS} 

LDFLAGS =

COMMON_OBJECTS = $(COMMON_SOURCES:.c=.o)


test_gpio: $(COMMON_OBJECTS) test_gpio.o
	$(CC)  $(LDFLAGS) $(COMMON_OBJECTS) test_gpio.o -o test_gpio

.c.o:
	$(CC) -c $(CCFLAGS) $< -o $@


clean: 
	rm -f *.o   ${EXEC} *~  test_gpio

indent:
	indent -bad -bap -nbc -bl -nce -i2 --no-tabs --line-length120 $(COMMON_SOURCES) $(H_SOURCES)
