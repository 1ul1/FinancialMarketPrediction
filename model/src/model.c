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

    print_skill();
    populate_error_metrics();

    train(company, market, weights);
    print_skill();

    populate();
}
