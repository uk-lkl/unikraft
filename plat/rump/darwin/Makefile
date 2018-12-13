COMPILE.c?=$(CC) -c $(CPPFLAGS) $(CFLAGS)
COMPILE.S?=$(CC) -c $(CPPFLAGS) $(ASFLAGS)
RUMPOBJ?=../../rumpobj
OBJDIR=$(RUMPOBJ)/platform
RUMP?=$(RUMPOBJ)/rump
CRTDIR=$(RUMP)/lib
OBJ=$(CRTDIR)/crt1.o
OBJ+=$(OBJDIR)/clock_getres.o
OBJ+=$(OBJDIR)/clock_gettime.o $(OBJDIR)/gettimeofday.o
OBJ+=$(OBJDIR)/clock_nanosleep.o $(OBJDIR)/nanosleep.o
OBJ+=$(OBJDIR)/fcntl.o
OBJ+=$(OBJDIR)/fstat.o $(OBJDIR)/fsync.o
OBJ+=$(OBJDIR)/getpagesize.o $(OBJDIR)/getrandom.o
OBJ+=$(OBJDIR)/ioctl.o $(OBJDIR)/kill.o
OBJ+=$(OBJDIR)/lseek.o $(OBJDIR)/pipe.o
OBJ+=$(OBJDIR)/mmap.o $(OBJDIR)/munmap.o
OBJ+=$(OBJDIR)/poll.o $(OBJDIR)/select.o
OBJ+=$(OBJDIR)/pread.o $(OBJDIR)/pwrite.o
OBJ+=$(OBJDIR)/preadv.o $(OBJDIR)/pwritev.o
OBJ+=$(OBJDIR)/read.o $(OBJDIR)/readv.o
OBJ+=$(OBJDIR)/write.o $(OBJDIR)/writev.o
OBJ+=$(OBJDIR)/_start1.o $(OBJDIR)/_exit.o
OBJ+=$(OBJDIR)/sysctl.o
OBJ+=$(OBJDIR)/sigaction.o $(OBJDIR)/sigprocmask.o
OBJ+=$(OBJDIR)/mach_absolute_time.o
HEADERS=syscall.h i386/syscall.h x86_64/syscall.h darwin.h
INCLUDES=-I../include -I../../franken/include -I../../franken/init -I../../franken/ucontext -I../../librumpuser -I../../franken/thread -I../../franken/ucontext/include
CPPFLAGS+=-nostdinc $(INCLUDES)
CFLAGS+=-fno-stack-protector -O0 -g3

default:			$(OBJDIR) $(CRTDIR) $(OBJ)

$(OBJDIR):
				mkdir -p $(OBJDIR)

$(CRTDIR):
				mkdir -p $(CRTDIR)

$(CRTDIR)/crt1.o:		crt1.S x86_64/crt1.S i386/crt1.S
				$(COMPILE.S) -o $@ crt1.S
				cp $@ $(CRTDIR)/crt0.o

$(OBJDIR)/clock_getres.o:	clock_getres.c $(HEADERS)
				$(COMPILE.c) -o $@ clock_getres.c

$(OBJDIR)/clock_gettime.o:	clock_gettime.c $(HEADERS)
				$(COMPILE.c) -o $@ clock_gettime.c

$(OBJDIR)/clock_nanosleep.o:	clock_nanosleep.c $(HEADERS)
				$(COMPILE.c) -o $@ clock_nanosleep.c

$(OBJDIR)/gettimeofday.o:	gettimeofday.c $(HEADERS)
				$(COMPILE.c) -o $@ gettimeofday.c

$(OBJDIR)/nanosleep.o:		nanosleep.c $(HEADERS)
				$(COMPILE.c) -o $@ nanosleep.c

$(OBJDIR)/_exit.o:		_exit.c $(HEADERS)
				$(COMPILE.c) -o $@ _exit.c

$(OBJDIR)/fcntl.o:		fcntl.c $(HEADERS)
				$(COMPILE.c) -o $@ fcntl.c

$(OBJDIR)/fstat.o:		fstat.c $(HEADERS)
				$(COMPILE.c) -o $@ fstat.c

$(OBJDIR)/fsync.o:		fsync.c $(HEADERS)
				$(COMPILE.c) -o $@ fsync.c

$(OBJDIR)/getpagesize.o:	getpagesize.c $(HEADERS)
				$(COMPILE.c) -o $@ getpagesize.c

$(OBJDIR)/getrandom.o:		getrandom.c $(HEADERS)
				$(COMPILE.c) -o $@ getrandom.c

$(OBJDIR)/ioctl.o:		ioctl.c $(HEADERS)
				$(COMPILE.c) -o $@ ioctl.c

$(OBJDIR)/kill.o:		kill.c $(HEADERS)
				$(COMPILE.c) -o $@ kill.c

$(OBJDIR)/lseek.o:		lseek.c $(HEADERS)
				$(COMPILE.c) -o $@ lseek.c

$(OBJDIR)/mach_absolute_time.o: mach_absolute_time.s
				$(COMPILE.S) -march=native -o $@ $^

$(OBJDIR)/mmap.o:		mmap.c $(HEADERS)
				$(COMPILE.c) -o $@ mmap.c

$(OBJDIR)/mprotect.o:		mprotect.c $(HEADERS)
				$(COMPILE.c) -o $@ mprotect.c

$(OBJDIR)/munmap.o:		munmap.c $(HEADERS)
				$(COMPILE.c) -o $@ munmap.c

$(OBJDIR)/pipe.o:		pipe.c $(HEADERS)
				$(COMPILE.c) -o $@ pipe.c

$(OBJDIR)/poll.o:		poll.c $(HEADERS)
				$(COMPILE.c) -o $@ poll.c

$(OBJDIR)/pread.o:		pread.c $(HEADERS)
				$(COMPILE.c) -o $@ pread.c

$(OBJDIR)/preadv.o:		preadv.c $(HEADERS)
				$(COMPILE.c) -o $@ preadv.c

$(OBJDIR)/pwrite.o:		pwrite.c $(HEADERS)
				$(COMPILE.c) -o $@ pwrite.c

$(OBJDIR)/pwritev.o:		pwritev.c $(HEADERS)
				$(COMPILE.c) -o $@ pwritev.c

$(OBJDIR)/read.o:		read.c $(HEADERS)
				$(COMPILE.c) -o $@ read.c

$(OBJDIR)/readv.o:		readv.c $(HEADERS)
				$(COMPILE.c) -o $@ readv.c

$(OBJDIR)/select.o:		select.c $(HEADERS)
				$(COMPILE.c) -o $@ select.c

$(OBJDIR)/_start1.o:		_start1.c $(HEADERS)
				$(COMPILE.c) -o $@ _start1.c

$(OBJDIR)/sysctl.o:		sysctl.c $(HEADERS)
				$(COMPILE.c) -o $@ sysctl.c

$(OBJDIR)/sigaction.o:		sigaction.c $(HEADERS)
				$(COMPILE.c) -o $@ sigaction.c

$(OBJDIR)/sigprocmask.o:	sigprocmask.c $(HEADERS)
				$(COMPILE.c) -o $@ sigprocmask.c

$(OBJDIR)/write.o:		write.c $(HEADERS)
				$(COMPILE.c) -o $@ write.c

$(OBJDIR)/writev.o:		writev.c $(HEADERS)
				$(COMPILE.c) -o $@ writev.c

.PHONY:				clean
clean:
				rm -f $(OBJDIR)/*.o $(CRTDIR)/*.o
