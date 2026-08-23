#include "utils.h"

void populate() {
    if (PREDICTION->days[0].bias == 0 && PREDICTION->days[0].sd == 0) {exit(1);}
    
    double* features = (double*)malloc(sizeof(double) * NR_FEATURES);
    double* computed = (double*)malloc(sizeof(double) * PREDICTION->len_days);
    int last = LAST;
    
    calculate_features(features, COMPANY, last);
    predict(computed, features);

    PREDICTION->today= TODAY;

    for (int i = 0; i < PREDICTION->len_days; i += 1) {
        PREDICTION->days[i].starting_price = COMPANY->samples[last].c;
        PREDICTION->days[i].expected_return = computed[i] + PREDICTION->days[i].bias;
        PREDICTION->days[i].expected_price = COMPANY->samples[last].c * exp(computed[i] + PREDICTION->days[i].bias);
        PREDICTION->days[i].forecast_strength = (
            (computed[i] + PREDICTION->days[i].bias) /  PREDICTION->days[i].sd
        );
        // Conversion to $
        PREDICTION->days[i].sd = COMPANY->samples[last].c / 2 * (
                exp(PREDICTION->days[i].expected_return + PREDICTION->days[i].sd)
                -
                exp(PREDICTION->days[i].expected_return - PREDICTION->days[i].sd)
        );
        PREDICTION->days[i].bias = COMPANY->samples[last].c * (exp(PREDICTION->days[i].bias) - 1);
    }
    
    
    free(features);
    free(computed);
}

void populate_error_metrics() {
    double* features = (double*)malloc(sizeof(double) * NR_FEATURES);
    double* computed = (double*)malloc(sizeof(double) * PREDICTION->len_days);
    double* expected = (double*)malloc(sizeof(double) * PREDICTION->len_days);

    // -------------------------------------------------------------------------------------------
    // BIAS
    for (int time = 20; time < LAST + 1; time += 1) {
        calculate_features(features, COMPANY, time);
        predict(computed, features);
        expect(expected, time);

        if (time + 1 < LAST + 1) {
            PREDICTION->days[0].bias += expected[0] - computed[0];
        }
        if (time + 5 < LAST + 1) {
            PREDICTION->days[1].bias += expected[1] - computed[1];
        }
        if (time + 10 < LAST + 1) {
            PREDICTION->days[2].bias += expected[2] - computed[2];
        }
        if (time + 20 < LAST + 1) {
            PREDICTION->days[3].bias += expected[3] - computed[3];
        }
    }

    // BIAS average
    PREDICTION->days[0].bias /= (LAST + 1 - 21);
    PREDICTION->days[1].bias /= (LAST + 1 - 25);
    PREDICTION->days[2].bias /= (LAST + 1 - 30);
    PREDICTION->days[3].bias /= (LAST + 1 - 40);

    // Standard Deviation
    for (int time = 20; time < LAST + 1; time += 1) {
        calculate_features(features, COMPANY, time);
        predict(computed, features);
        expect(expected, time);

        if (time + 1 < LAST + 1) {
            PREDICTION->days[0].sd += pow(expected[0] - computed[0] - PREDICTION->days[0].bias, 2);
        }
        if (time + 5 < LAST + 1) {
            PREDICTION->days[1].sd += pow(expected[1] - computed[1] - PREDICTION->days[1].bias, 2);
        }
        if (time + 10 < LAST + 1) {
            PREDICTION->days[2].sd += pow(expected[2] - computed[2] - PREDICTION->days[2].bias, 2);
        }
        if (time + 20 < LAST + 1) {
            PREDICTION->days[3].sd += pow(expected[3] - computed[3] - PREDICTION->days[3].bias, 2);
        }
    }

    PREDICTION->days[0].sd /= (LAST + 1 - 21);
    PREDICTION->days[1].sd /= (LAST + 1 - 25);
    PREDICTION->days[2].sd /= (LAST + 1 - 30);
    PREDICTION->days[3].sd /= (LAST + 1 - 40);
    
    PREDICTION->days[0].sd = sqrt(PREDICTION->days[0].sd);
    PREDICTION->days[1].sd = sqrt(PREDICTION->days[1].sd);
    PREDICTION->days[2].sd = sqrt(PREDICTION->days[2].sd);
    PREDICTION->days[3].sd = sqrt(PREDICTION->days[3].sd);

    free(features);
    free(computed);
    free(expected);
}
