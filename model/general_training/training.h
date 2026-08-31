#ifndef TRAINING_H
#define TRAINING_H

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

extern Company* MARKET;

typedef struct Companies {
    int len_companies;
    Company* companies;
} Companies;

extern Companies* COMPANIES;

typedef struct Weights {
    int len_weights;
    double* weights;
    int len_bias;
    double* bias;
    double* means;
    double* standard_deviations;
} Weights;

extern Weights* WEIGHTS;
extern int NR_FEATURES;
extern int NR_COMPANIES;
extern double ALPHA;

void calculate_raw_features(double* features, const Company* company, int time);
void calculate_features(double*** features);

// ---------------------------------------------------------------------------
//                                                               AUX Functions
// ---------------------------------------------------------------------------

static inline double get_field(const Sample* sample, size_t offset) {
    return *((const double*)((const char*)sample + offset));
}

static inline double sum(
    int start,
    int end,
    int step,
    const Company* over,
    size_t offset
) {
    double ans = 0;
    while (start < end) {
        ans += get_field((*over).samples + start, offset);
        
        start += step;
    }
    return ans;
}

static inline double average_dollar_trade_size(
    int start,
    int end,
    const Company* over,
    int full_price
) {
    double numerator = 0, denominator = 0, count = end - start;

    while (start < end) {
        numerator += over->samples[start].vw * over->samples[start].v;
        denominator += over->samples[start].n;
        
        start += 1;
    }

    return (full_price == 0) ? numerator / denominator : numerator / count;
}

// ln(Ct/Ct-1) for all 3 to measure relative changes
// so price itself is not indicative
static inline double volatility(
    int start,
    int end,
    const Company* over
) {
    // Just Standard Deviation of returns
    // How much returns overall flunctuate around the mean of ln(Ct/Ct-1)
    double
        ans = 0,
        count = end - start,
        mean = log(over->samples[end - 1].c / over->samples[start - 1].c);
    mean /= count;

    while (start < end) {
        ans += pow(
            log(over->samples[start].c / over->samples[start - 1].c)
            -
            mean,
            2
        );
        
        start += 1;
    }

    ans /= count;

    return sqrt(ans);
}

static inline double dispertion(
    int start,
    int end,
    const Company* over
) {
    // Just Standard Deviation of closing prices
    // How dispersed current raw prices are from their own mean
    // How stretched out the entire price path is
    double  ans = 0,
            mean = 0;
    int     count = end - start;

    while (start < end) {
        mean += over->samples[start].c;
        start += 1;
    }
    mean /= count;
    
    start = end - count;
    while (start < end) {
        ans += pow(over->samples[start].c - mean, 2);
        start += 1;
    }

    ans /= count;
    
    return sqrt(ans) / mean;
}

static inline double stability(
    int start,
    int end,
    const Company* over
) {
    // High values mean ln(Ct/Ct-1) varies a lot from day to day
    // Low values mean it is stable, the change is stable
    // 0 means full stability
    start += 1;
    
    double  ans = 0,
            count = end - start;

    while (start < end) {
        ans += pow(
            log(over->samples[start].c / over->samples[start - 1].c)
            -
            log(over->samples[start - 1].c / over->samples[start - 2].c),
            2
        );
        
        start += 1;
    }

    ans /= count;

    return sqrt(ans);
}

static inline double persistence(
    int start,
    int end,
    const Company* over
) {
    // Pearson correlation coefficient
    // Covariance(X, Y) / (Standard Deviation X * Standard Deviation Y)
    // High Values mean returns tend to continue
    // Low Values mean retturns tend to reverse
    // 0 means little relationship between returns
    double
        mean_X = log(over->samples[end - 2].c / over->samples[start - 1].c),
        mean_Y = log(over->samples[end - 1].c / over->samples[start].c);
    mean_X /= end - start - 1;
    mean_Y /= end - start - 1;
    
    double
        stddev_X = volatility(start, end - 1, over),
        stddev_Y = volatility(start + 1, end, over);

    double numerator = 0, denominator = stddev_X * stddev_Y;

    for (int i = start; i < end - 1; i += 1) {
        numerator += (
            (log(over->samples[i].c / over->samples[i - 1].c) - mean_X)
            *
            (log(over->samples[i + 1].c / over->samples[i].c) - mean_Y)
        );
    }

    numerator /= (end - start - 1);

    return (denominator == 0) ? 0 : numerator / denominator;
}

static inline double return_volatility_relative_to_market(
    int start,
    int end,
    const Company* over,
    const Company* market
) {
    // Just Standard Deviation but against market mean
    double  ans = 0,
            count = end - start,
            mean = (
                log(market->samples[end - 1].c / market->samples[start - 1].c)
            );
    mean /= count;

    while (start < end) {
        ans += pow(
            log(over->samples[start].c / over->samples[start - 1].c)
            -
            mean
            ,
            2
        );
        
        start += 1;
    }

    ans /= count;

    return sqrt(ans);
}

static inline double return_covariance_to_market(
    int start,
    int end,
    const Company* over,
    const Company* market
) {
    // Pearson but against the market
    // Its just correlation to the market
    double  ans = 0,
            count = end - start,
            mean_X = log(
                over->samples[end - 1].c / over->samples[start - 1].c
            ) / (end - start),
            mean_Y = log(
                market->samples[end - 1].c / market->samples[start - 1].c
            ) / (end - start);

    while (start < end) {
        double  curr_X = log(
                    over->samples[start].c / over->samples[start - 1].c
                ) - mean_X,
                curr_Y = log(
                    market->samples[start].c / market->samples[start - 1].c
                ) - mean_Y;

        ans += curr_X * curr_Y;
            
        start += 1;
    }

    ans /= count;
    
    return ans;
}

// ---------------------------------------------------------------------------
//                                                             Verift Training
// ---------------------------------------------------------------------------

static inline void predict(double* res, double* features) {

    res[0] = 0;
    res[1] = 0;
    res[2] = 0;
    res[3] = 0;

    for (int idx = 0; idx < 4; idx += 1) {
        
        for (int i = 0; i < NR_FEATURES; i += 1) {
            res[idx] += WEIGHTS->weights[i + idx * NR_FEATURES] * features[i];
        }
        res[idx] += WEIGHTS->bias[idx];
    }
}

static inline void expect(double* res, int time, Company* company) {

    res[0] = 0;
    res[1] = 0;
    res[2] = 0;
    res[3] = 0;

    if (time + 1 < company->count) {
        res[0] = log(company->samples[time + 1].c  / company->samples[time].c);
    }
    if (time + 5 < company->count) {
        res[1] = log(company->samples[time + 5].c  / company->samples[time].c); 
    }
    if (time + 10 < company->count) {
        res[2] = log(company->samples[time + 10].c / company->samples[time].c);
    }
    if (time + 20 < company->count) {
        res[3] = log(company->samples[time + 20].c / company->samples[time].c);
    }
}


static inline void error(double* ans, double*** features) {
    double* computed = (double*)malloc(sizeof(double) * 4);
    double* expected = (double*)malloc(sizeof(double) * 4);

    for (int time = 20; time < MARKET->count - 20; time += 1) {
        for (int i = 0; i < NR_COMPANIES; i += 1) {
            predict(computed, features[time][i]);
            expect(expected, time, &(COMPANIES->companies[i]));

            for (int idx = 0; idx < 4; idx += 1) {
                ans[idx] += pow(computed[idx] - expected[idx], 2);
            }
        }
    }

    for (int idx = 0; idx < 4; idx += 1) {
        ans[idx] /= (MARKET->count - 40) * NR_COMPANIES;
        ans[idx] = sqrt(ans[idx]);
    }

    free(computed);
    free(expected);
}

// ---------------------------------------------------------------------------
//                                                           Feature Centering
// ---------------------------------------------------------------------------

#endif
