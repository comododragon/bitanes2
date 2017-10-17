ifeq ($(DEBUG),yes)
    DEBUGFLAG=-g
endif

ifeq ($(GPROF),yes)
    PROFFLAG=-pg
endif

ifeq ($(GCOV),yes)
    PROFFLAG=-fprofile-arcs -ftest-coverage
endif

ifeq ($(OPTLEVEL),1)
    DEFFLAG+= -DGRAPH_USE_ADJ_MATRIX
else ifeq ($(OPTLEVEL),2)
    DEFFLAG+= -DGRAPH_USE_GET_ADJACENTS
else
    DEFFLAG+= -DGRAPH_USE_ADJ_MATRIX -DLIST_DISABLE_TAIL
endif

CFLAGS=-O3 -Wall

bin/bitanes2: src/bitanes2.c obj/graph.o include/graph.h obj/list.o include/list.h include/common/common.h
	mkdir -p bin
	$(CC) src/bitanes2.c obj/graph.o obj/list.o -Iinclude $(DEFFLAG) -lm -o $@ $(DEBUGFLAG) $(CFLAGS) $(PROFFLAG)

obj/graph.o: src/graph.c include/graph.h
	mkdir -p obj
	$(CC) -c $< -Iinclude $(DEFFLAG) -lm -o $@ $(DEBUGFLAG) $(CFLAGS) $(PROFFLAG)

obj/list.o: src/list.c include/list.h
	mkdir -p obj
	$(CC) -c $< -Iinclude $(DEFFLAG) -o $@ $(DEBUGFLAG) $(CFLAGS) $(PROFFLAG)

clean:
	rm -rf obj
	rm -rf bin
