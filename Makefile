ifeq ($(DEBUG),yes)
    DEBUGFLAG=-g
endif

ifeq ($(GPROF),yes)
    PROFFLAG=-pg
endif

ifeq ($(GCOV),yes)
    PROFFLAG=-fprofile-arcs -ftest-coverage
endif

CFLAGS=-O3 -Wall

bin/bitanes2: src/bitanes2.c obj/graph.o include/graph.h obj/list.o include/list.h include/common/common.h
	mkdir -p bin
	mpicc src/bitanes2.c obj/graph.o obj/list.o -Iinclude $(DEFFLAG) -lm -o $@ $(DEBUGFLAG) $(CFLAGS) $(PROFFLAG)

bin/bitanes2b: src/bitanes2b.c obj/graph.o include/graph.h obj/list.o include/list.h include/common/common.h
	mkdir -p bin
	mpicc src/bitanes2b.c obj/graph.o obj/list.o -Iinclude $(DEFFLAG) -lm -o $@ $(DEBUGFLAG) $(CFLAGS) $(PROFFLAG)

obj/graph.o: src/graph.c include/graph.h
	mkdir -p obj
	mpicc -c $< -Iinclude $(DEFFLAG) -lm -o $@ $(DEBUGFLAG) $(CFLAGS) $(PROFFLAG)

obj/list.o: src/list.c include/list.h
	mkdir -p obj
	mpicc -c $< -Iinclude $(DEFFLAG) -o $@ $(DEBUGFLAG) $(CFLAGS) $(PROFFLAG)

clean:
	rm -rf obj
	rm -rf bin
