#include "utils.h"

void adjust_updates(const Company* company,
    double* updates,
    const double* features,
    int time,
    int prediction_time,
    int start,
    int end
) {

    double prediction = 0, expected = log(company->samples[prediction_time].c / company->samples[time].c);
   
    for (int i = start; i < end; i += 1) {
        prediction += WEIGHTS->weights[i] * features[i - start];
    }
    prediction += WEIGHTS->bias[start / NR_FEATURES];

    double error = prediction - expected;


    for (int i = start; i < end; i += 1) {
        updates[i] += error * features[i - start] + LAMBDA * WEIGHTS->weights[i];
    }
    updates[WEIGHTS->len_weights + start / NR_FEATURES] += error;
}

void process_one_company(const Company* company, double* updates, int time, double* features) {

    if (LAST + 1 - time > 1) {
        int start = 0, end = NR_FEATURES;
        adjust_updates(company, updates, features, time, time + 1, start, end);
    }

    if (LAST + 1 - time > 5) {
        int start = NR_FEATURES, end = NR_FEATURES * 2;
        adjust_updates(company, updates, features, time, time + 5, start, end);
    }

    if (LAST + 1 - time > 10) {
        int start = NR_FEATURES * 2, end = NR_FEATURES * 3;
        adjust_updates(company, updates, features, time, time + 10, start, end);
    }

    if (LAST + 1 - time > 20) {
        int start = NR_FEATURES * 3, end = NR_FEATURES * 4;
        adjust_updates(company, updates, features, time, time + 20, start, end);
    }
}

void train(
    const Company company,
    const Company market,
    Weights weights,
    double** features
) {
    for (int k = 0; k < EPOCHS; k += 1) {

        double alpha = ALPHA * exp((-1) * BETA * k);
            
        for (int time = 20; time < LAST + 1; time += 1) {
            
            double* updates = (double*)calloc(WEIGHTS->len_weights + WEIGHTS->len_bias, sizeof(double));

            // calculate gi for each wi and save it in updates
            process_one_company(&company, updates, time, features[time]);

            // update wi using -= ALPHA * gi
            for (int i = 0; i < WEIGHTS->len_weights; i += 1) {
                WEIGHTS->weights[i] -= alpha * updates[i];
            }
            for (int i = 0; i < WEIGHTS->len_bias; i += 1) {
                WEIGHTS->bias[i] -= alpha * updates[WEIGHTS->len_weights + i];
            }

            //print_weights();
            
            free(updates);
        }
    }
}
