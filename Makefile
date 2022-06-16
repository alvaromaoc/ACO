TARGET = main dr-benchmark pk-benchmark
CXXFLAGS = -fopenmp -O3
SOURCE = src
CLASSES = $(SOURCE)/classes
OBJS = $(CLASSES)/Algorithms.o $(CLASSES)/InfluenceGraph.o $(CLASSES)/Population.o $(CLASSES)/InputOutputUtils.o $(CLASSES)/Roulette.o $(CLASSES)/UniqueQueue.o $(CLASSES)/Algorithm.o $(CLASSES)/AppConfig.o
CCC = g++

all: ${TARGET}

main: main.o $(OBJS)
	$(CCC) $(CXXFLAGS) $(SOURCE)/main.o $(OBJS) -o main

main.o: $(SOURCE)/main.cpp
	$(CCC) -c $(CXXFLAGS) $(SOURCE)/main.cpp -o $(SOURCE)/main.o

dr-benchmark: dr-benchmark.o $(OBJS)
	$(CCC) $(CXXFLAGS) $(SOURCE)/dr-benchmark.o $(OBJS) -o dr-benchmark

dr-benchmark.o: $(SOURCE)/dr-benchmark.cpp
	$(CCC) -c $(CXXFLAGS) $(SOURCE)/dr-benchmark.cpp -o $(SOURCE)/dr-benchmark.o

pk-benchmark: pk-benchmark.o $(OBJS)
	$(CCC) $(CXXFLAGS) $(SOURCE)/pk-benchmark.o $(OBJS) -o pk-benchmark

pk-benchmark.o: $(SOURCE)/pk-benchmark.cpp
	$(CCC) -c $(CXXFLAGS) $(SOURCE)/pk-benchmark.cpp -o $(SOURCE)/pk-benchmark.o

clean:
	@rm -f $(SOURCE)/*.o ${TARGET} core
	@rm -f $(CLASSES)/*.o ${TARGET} core