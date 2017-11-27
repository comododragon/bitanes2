ifeq ($(DEBUG),yes)
    DEBUGFLAG=-g
endif

ifeq ($(GPROF),yes)
    PROFFLAG=-pg
endif

ifeq ($(GCOV),yes)
    PROFFLAG=-fprofile-arcs -ftest-coverage
endif

CFLAGS=-O3 -Wall -fopenmp

bin/bitanes2: src/bitanes2.c obj/graph.o include/graph.h obj/list.o include/list.h include/common/common.h
	mkdir -p bin
	$(CC) src/bitanes2.c obj/graph.o obj/list.o -Iinclude -lm -o $@ $(DEBUGFLAG) $(CFLAGS) $(PROFFLAG)

obj/graph.o: src/graph.c include/graph.h
	mkdir -p obj
	$(CC) -c $< -Iinclude -lm -o $@ $(DEBUGFLAG) $(CFLAGS) $(PROFFLAG)

obj/list.o: src/list.c include/list.h
	mkdir -p obj
	$(CC) -c $< -Iinclude -o $@ $(DEBUGFLAG) $(CFLAGS) $(PROFFLAG)

clean:
	rm -rf obj
	rm -rf bin
