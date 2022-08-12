CXX = g++ -std=c++11
WARN = -Wall
CFLAGS = $(WARN)
LDFLAGS = $(WARN) -lecf

OBJS = FunctionMinEvalOp.o \
			 ActivationFunctions/AbstractActivationFunction.o \
			 ActivationFunctions/LinearFunction.o \
			 ActivationFunctions/SigmoidFunction.o \
			 ActivationFunctions/SineFunction.o \
			 NeuralNetwork.o \
			 Crossovers/CrsLayer.o \
			 Crossovers/CrsNeuron.o \
			 Crossovers/CrsNeuronUniform.o \
			 Mutations/MutNeuron.o \
			 Mutations/MutNeuronN.o \
			 Mutations/MutScale.o \
			 Algorithms/Backpropagation.o \
       main.o

BINFILE = neural

.PHONY = all clean

all: $(BINFILE)

$(BINFILE): $(OBJS)
	$(CXX) $(OBJS) $(LDFLAGS) -o $(BINFILE)

%.o: %.cpp FunctionMinEvalOp.h
	$(CXX) $(CFLAGS) $< -c -o $@

clean:
	rm -f *~ $(OBJS) $(BINFILE)
