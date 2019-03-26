OBJS = main.o htable.o
TARGET = os2

.SUFFIXES : .c .o

all : $(TARGET)

$(TARGET) : $(OBJS)
	$(CC) -o $@ $^ -lpthread

clean:
	rm -f $(OBJS) $(TARGET)

main.o	 : main.c htable.h
htable.o : htable.c htable.h list.h
