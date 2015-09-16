CCFLAGS=-Iinclude -Isrc --std-c99
LDFLAGS= -Lsrc
PROGRAMS = main
SOURCES = ${PROGRAMS:=.c}

all: ${PROGRAMS}

${PROGRAMS}: ${SOURCES}
	"C:\Program Files\SDCC\bin\sdcc" --model-large $(CCFLAGS) $(LDFLAGS) main.c
clean:
	del main.asm  main.cdb  main.ihx  main.lk  main.lst  main.map  main.mem  main.omf  main.rel  main.rst  main.sym
