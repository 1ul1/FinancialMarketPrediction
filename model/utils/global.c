#include "utils.h"

double ALPHA = 0.01;
double BETA = 0;
double LAMBDA = 0.001;

int EPOCHS = 100;

int TODAY = 0;
int LAST = 0;

int NR_FEATURES = 68;

int NR_THREADS = 12;

int get_nr_features() {
    return NR_FEATURES;
}

int get_nr_models() {
    return 4;
}

