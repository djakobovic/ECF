CXX = g++
WARN = -Wall
CFLAGS = $(WARN)
LDFLAGS = $(WARN) -lecf

OBJS = AntEvalOp.o \
       main.o

BINFILE = ant

.PHONY = all clean

all: $(BINFILE)

$(BINFILE): $(OBJS)
	$(CXX) $(OBJS) $(LDFLAGS) -o $(BINFILE)

%.o: %.cpp AntEvalOp.h IfFoodAhead.h MoveAhead.h Prog2.h Prog3.h TurnLeft.h TurnRight.h
	$(CXX) $(CFLAGS) $< -c -o $@

clean:
	rm -f *~ $(OBJS) $(BINFILE)
