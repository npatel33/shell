EXEC =		sh

OBJS = 		sh.o

IFLAGS =	

LFLAGS = 	

RFLAGS =	

LIBS =		

CFLAGS =	$(AUXCFLAGS) -Wall -pedantic

AUXCFLAGS =	-g

CC =		/usr/bin/gcc


all: $(EXEC)

fast::	
		$(MAKE) $(MFLAGS) AUXCFLAGS="-O3"

noprompt::	
		$(MAKE) $(MFLAGS) AUXCFLAGS="-DNO_PROMPT"

profile::
		$(MAKE) $(MFLAGS) AUXCFLAGS="-O3 -pg"

$(EXEC): $(OBJS)
		$(CC) -o $@ $(CFLAGS) $(OBJS) $(LFLAGS) $(RFLAGS) $(LIBS)



clean::
		rm -f *.o $(OBJS) $(EXEC) *~ cscope.out mon.out gmon.out


lint::
		lint $(IFLAGS) $(LFLAGS) $(LIBS) $(OBJS:%.o=%.c)



.c.o:
		$(CC) $(CFLAGS) $(IFLAGS) -c $<

%.c:	%.h

