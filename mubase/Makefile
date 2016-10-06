#target: dependencies
#[tab] system command

# Dirs
INCDIR = ./include
SRCDIR = ./src
OBJDIR = ./obj
BINDIR = ./bin

# Header files
GLOBAL_H = $(INCDIR)/global.h
UTIL_H = $(INCDIR)/util.h
DM_H = $(INCDIR)/dm.h
BM_H = $(INCDIR)/bm.h
BT_H = $(INCDIR)/bt.h
ITER_H = $(INCDIR)/iter.h
HEAP_H = $(INCDIR)/heap.h
RM_H = $(INCDIR)/rm.h
SM_H = $(INCDIR)/sm.h
SORT_H = $(INCDIR)/sort.h
SP_H = $(INCDIR)/sp.h

# Source files
UTIL_SRC = $(SRCDIR)/util.cpp
DM_SRC = $(SRCDIR)/dm.cpp
BM_SRC = $(SRCDIR)/bm.cpp
BT_SRC = $(SRCDIR)/bt.cpp
ITER_SRC = $(SRCDIR)/iter.cpp
RM_SRC = $(SRCDIR)/rm.cpp
SM_SRC = $(SRCDIR)/sm.cpp
SORT_SRC = $(SRCDIR)/sort.cpp
SP_SRC = $(SRCDIR)/sp.cpp
TEST_SRC = $(SRCDIR)/test.cpp

# Obj files
UTIL_O = $(OBJDIR)/util.o
DM_O = $(OBJDIR)/dm.o
BM_O = $(OBJDIR)/bm.o
BT_O = $(OBJDIR)/bt.o
ITER_O = $(OBJDIR)/iter.o
RM_O = $(OBJDIR)/rm.o
SM_O = $(OBJDIR)/sm.o
SORT_O = $(OBJDIR)/sort.o
SP_O = $(OBJDIR)/sp.o
TEST_O = $(OBJDIR)/test.o

# General
INPUT = input
TEMP = bin
CFLAGS = -c -I $(INCDIR)
CC = g++
CP = cp

all: $(BINDIR)/exec

EXEC_DEPEND = $(TEST_O) $(UTIL_O) $(DM_O) $(BM_O) \
			  $(SP_O) $(SM_O) $(BT_O) $(RM_O)

$(BINDIR)/exec : $(EXEC_DEPEND)
		$(CC) $(EXEC_DEPEND) -o $(BINDIR)/exec

$(TEST_O): $(TEST_SRC) $(DM_H) $(BM_H) $(RM_H)
		$(CC) $(CFLAGS) -o $(TEST_O) $(TEST_SRC)
		
$(UTIL_O)	:	$(UTIL_SRC) $(UTIL_H)
		$(CC) $(CFLAGS) -o $(UTIL_O) $(UTIL_SRC)
		
$(DM_O)	:	$(DM_SRC) $(DM_H)
		$(CC) $(CFLAGS) -o $(DM_O) $(DM_SRC)

$(BM_O)	:	$(BM_SRC) $(BM_H) $(DM_H)
		$(CC) $(CFLAGS) -o $(BM_O) $(BM_SRC)
		
$(SP_O):	$(SP_SRC) $(SP_H) $(UTIL_H)
		$(CC) $(CFLAGS) -o $(SP_O) $(SP_SRC)
		
$(SM_O)	:	$(SM_SRC) $(SM_H) $(GLOBAL_H) $(DM_H) $(BM_H) \
			$(SP_H) $(UTIL_H)
		$(CC) $(CFLAGS) -o $(SM_O) $(SM_SRC)
		
$(BT_O)	:	$(BT_SRC) $(BT_H) $(SM_H)
		$(CC) $(CFLAGS) -o $(BT_O) $(BT_SRC)
		
$(RM_O): $(RM_SRC) $(RM_H) $(ITER_H) $(GLOBAL_H) $(BM_H) \
		 $(SM_H) $(UTIL_H) $(SP_H) $(BT_H)
		$(CC) $(CFLAGS) -o $(RM_O) $(RM_SRC)
		
$(SORT_O):	$(SORT_SRC) $(RM_H) $(BM_H) $(SP_H) $(HEAP_H)
		$(CC) $(CFLAGS) -o $(SORT_O) $(SORT_SRC)
		
run: 
	./bin/exec
	
