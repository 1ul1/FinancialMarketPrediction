#ifndef UTILS_H
#define UTILS_H

#include <stddef.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

// ---------------------------------------------------------------------------
//                                                                  DATA Types
// ---------------------------------------------------------------------------

typedef struct Sample {
    double v;
    double vw;
    double o;
    double c;
    double h;
    double l;
    double t;
    double n;       
} Sample;

typedef struct Company {
    int count;
    Sample* samples;
} Company;

typedef struct Weights {
    int len_weights;
    double* weights;
    int len_bias;
    double* bias;
} Weights;

typedef struct Day {
    double bias;
    double sd;
    double expected_return;
    double expected_price;
    double starting_price;
    double forecast_strength;
} Day;

typedef struct Prediction {
    int today;
    int len_days;
    Day* days;
} Prediction;

void calculate_features(double* features, const Company* company, int time);

// ---------------------------------------------------------------------------
//                                                            Global Variables
// ---------------------------------------------------------------------------

Company* MARKET;
Company* COMPANY;
Weights* WEIGHTS;
Prediction* PREDICTION;

extern double ALPHA;
extern double BETA;
extern double LAMBDA;

extern int EPOCHS;

extern int TODAY;
extern int LAST;

extern int NR_FEATURES;

extern int NR_THREADS;

int get_nr_features();

int get_nr_models();


// ---------------------------------------------------------------------------
//                                                            Helper Functions
// ---------------------------------------------------------------------------
int get_nr_features();
int get_nr_models();

void print_weights();
int is_today(const Company* company);
double get_field(const Sample* sample, size_t offset);
double sum(
    int start,
    int end,
    int step,
    const Company* over,
    size_t offset
);
double average_dollar_trade_size(
    int start,
    int end,
    const Company* over,
    int full_price
);
double volatility(
    int start,
    int end,
    const Company* over
);
double dispertion(
    int start,
    int end,
    const Company* over
);
double stability(
    int start,
    int end,
    const Company* over
);
double persistence(
    int start,
    int end,
    const Company* over
);
double return_volatility_relative_to_market(
    int start,
    int end,
    const Company* over,
    const Company* market
);
double return_persistence_relative_to_market(
    int start,
    int end,
    const Company* over,
    const Company* market,
    double _standard_deviation,
    double _market_standard_deviation
);
void predict(double* res, double* features);
void expect(double* res, int time);
void error(double* ans);
void error_no_training(double* ans);

// ---------------------------------------------------------------------------
//                                                                  Prediction
// ---------------------------------------------------------------------------

void populate();
void populate_error_metrics();

// ---------------------------------------------------------------------------
//                                                                  Finetuning
// ---------------------------------------------------------------------------
void adjust_updates(const Company* company,
    double* updates,
    const double* features,
    int time,
    int prediction_time,
    int start,
    int end
);
void process_one_company(const Company* company, double* updates, int time);
void train(
    const Company company,
    const Company market,
    Weights weights
);
void print_skill();

#endif
