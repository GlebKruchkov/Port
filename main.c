// #include "ross.h"
#include "model.h"

int main(int argc, char* argv[]) {
	InitROSS();
    SimulateROSS(argc, argv);
	FinalizeROSS();
}