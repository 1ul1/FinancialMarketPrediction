#include "utils.h"

void start(
    Company company,
    Company market,
    Weights weights,
    Prediction* prediction
) {
    COMPANY = &company;
    MARKET = &market;
    WEIGHTS = &weights;
    PREDICTION = prediction;
    TODAY = is_today(COMPANY);
    LAST = TODAY ? COMPANY->count - 2: COMPANY->count - 1;

    double** features = (double**)malloc(sizeof(double*) * MARKET->count);
    for (int time = 20; time < MARKET->count; time += 1) {
        features[time] = (double*)malloc(sizeof(double) * NR_FEATURES);
    }
    calculate_features(features);
    
    print_skill(features);
    populate_error_metrics(features);

    train(company, market, weights, features);
    print_skill(features);

    populate(features[LAST]);

    for (int time = 20; time < MARKET->count; time += 1) {
        free(features[time]);
    }
    free(features);
}
