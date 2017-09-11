ifeq ($(DEBUG),yes)
    DEBUGFLAG=-g
endif

CFLAGS=-O3 -Wall

bin/bitanes2: src/bitanes2.c obj/graph.o include/graph.h obj/list.o include/list.h include/common/common.h
	mkdir -p bin
	$(CC) src/bitanes2.c obj/graph.o obj/list.o -Iinclude -o $@ $(DEBUGFLAG) $(CFLAGS)

obj/graph.o: src/graph.c include/graph.h
	mkdir -p obj
	$(CC) -c $< -Iinclude -o $@ $(DEBUGFLAG) $(CFLAGS)

obj/list.o: src/list.c include/list.h
	mkdir -p obj
	$(CC) -c $< -Iinclude -o $@ $(DEBUGFLAG) $(CFLAGS)

clean:
	rm -rf obj
	rm -rf bin
