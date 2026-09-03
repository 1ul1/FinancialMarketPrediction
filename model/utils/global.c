#include "utils.h"

Company* MARKET = NULL;
Company* COMPANY = NULL;
Weights* WEIGHTS = NULL;
Prediction* PREDICTION = NULL;

double ALPHA = 0.00001;
double BETA = 0.01;
double LAMBDA = 0.000005;

int EPOCHS = 100;

int TODAY = 0;
int LAST = 0;

int NR_FEATURES = 72;

int NR_THREADS = 12;

int get_nr_features() {
    return NR_FEATURES;
}

int get_nr_models() {
    return 4;
}

