#include "training.h"

Company* MARKET = NULL;
Weights* WEIGHTS = NULL;
Companies* COMPANIES = NULL;

double ALPHA = 0.001;
double BETA = 0.01;
int EPOCHS = 1;
double LAMBDA = 0.0005;

int NR_THREADS = 12;

int NR_FEATURES = 68;
int get_nr_features() {
    return NR_FEATURES;
}

int NR_COMPANIES = 0;

void calculate_features(double* features, const Company* company, int time) {

    if (time - 20 < 0) {exit(1);}

    Sample  s20 = company->samples[time - 20],
            s10 = company->samples[time - 10],
            s5  = company->samples[time - 5],
            s1  = company->samples[time - 1],
            s   = company->samples[time],
            
            market_s20 = MARKET->samples[time - 20],
            market_s10 = MARKET->samples[time - 10],
            market_s5  = MARKET->samples[time - 5],
            market_s1  = MARKET->samples[time - 1],
            market_s   = MARKET->samples[time];

    // --------------------------------------------------------------------------------
    //                                                                     Overall DATA
    // --------------------------------------------------------------------------------
    // Stock Momentum 5d 10d 20d
    // ln (C_t / C_(t - i))
    features[0] = log(s.c / s5.c );
    features[1] = log(s.c / s10.c);
    features[2] = log(s.c / s20.c);
    // --------------------------------------------------------------------------------
    // Market Momentum 5d 10d 20d
    // ln (C_t / C_(t - i))
    features[3] = log(market_s.c / market_s5.c);
    features[4] = log(market_s.c / market_s10.c);
    features[5] = log(market_s.c / market_s20.c);
    // --------------------------------------------------------------------------------
    // Stock Volume 5d 10d 20d
    double  v5  = sum(time - 4 , time + 1, 1, company, offsetof(Sample, v)) / 5,
            v10 = sum(time - 9 , time + 1, 1, company, offsetof(Sample, v)) / 10,
            v20 = sum(time - 19, time + 1, 1, company, offsetof(Sample, v)) / 20;
    features[6] = log(s.v / v5);
    features[7] = log(s.v / v10);
    features[8] = log(s.v / v20);
    // --------------------------------------------------------------------------------
    // Market Volume 5d 10d 20d
    double  market_v5  = sum(time - 4 , time + 1, 1, MARKET, offsetof(Sample, v)) / 5,
            market_v10 = sum(time - 9 , time + 1, 1, MARKET, offsetof(Sample, v)) / 10,
            market_v20 = sum(time - 19, time + 1, 1, MARKET, offsetof(Sample, v)) / 20;
    features[9]  = log(market_s.v / market_v5);
    features[10] = log(market_s.v / market_v10);
    features[11] = log(market_s.v / market_v20);
    // --------------------------------------------------------------------------------
    //                                                      Overall Movement Properties
    // --------------------------------------------------------------------------------
    // Stock Volatility 5d 10d 20d
    features[12] = volatility(time - 4 , time + 1, company);
    features[13] = volatility(time - 9 , time + 1, company);
    features[14] = volatility(time - 19, time + 1, company);
    // --------------------------------------------------------------------------------
    // Market Volatility 5d 10d 20d
    features[15] = volatility(time - 4 , time + 1, MARKET);
    features[16] = volatility(time - 9 , time + 1, MARKET);
    features[17] = volatility(time - 19, time + 1, MARKET);
    // --------------------------------------------------------------------------------
    // Stock Dispersion 5d 10d 20d
    features[18] = dispertion(time - 4 , time + 1, company);
    features[19] = dispertion(time - 9 , time + 1, company);
    features[20] = dispertion(time - 19, time + 1, company);
    // --------------------------------------------------------------------------------
    // Market Dispersion 5d 10d 20d
    features[21] = dispertion(time - 4 , time + 1, MARKET);
    features[22] = dispertion(time - 9 , time + 1, MARKET);
    features[23] = dispertion(time - 19, time + 1, MARKET);
    // --------------------------------------------------------------------------------
    // Stock Stability 5d 10d 20d
    features[24] = stability(time - 4 , time + 1, company);
    features[25] = stability(time - 9 , time + 1, company);
    features[26] = stability(time - 19, time + 1, company);
    // --------------------------------------------------------------------------------
    // Market Stability 5d 10d 20d
    features[27] = stability(time - 4 , time + 1, MARKET);
    features[28] = stability(time - 9 , time + 1, MARKET);
    features[29] = stability(time - 19, time + 1, MARKET);
    // --------------------------------------------------------------------------------
    // Stock Persistence 5d 10d 20d
    features[30] = persistence(time - 4 , time + 1, company);
    features[31] = persistence(time - 9 , time + 1, company);
    features[32] = persistence(time - 19, time + 1, company);
    // --------------------------------------------------------------------------------
    // Market Persistence 5d 10d 20d
    features[33] = persistence(time - 4 , time + 1, MARKET);
    features[34] = persistence(time - 9 , time + 1, MARKET);
    features[35] = persistence(time - 19, time + 1, MARKET);
    // --------------------------------------------------------------------------------
    //                                                                   Today's Action
    // --------------------------------------------------------------------------------
    // Gap | Stock & Market
    // ln(Current day Open Price / Previous day Close)
    features[36] = log(s.o / s1.c);
    features[37] = log(market_s.o / market_s1.c);
    // --------------------------------------------------------------------------------
    // Intraday Move | Stock & Market
    // ln(Close / Open)
    features[38] = log(s.c / s.o);
    features[39] = log(market_s.c / market_s.o);
    // --------------------------------------------------------------------------------
    // Range | Stock & Market
    // ln(high / low)
    features[40] = log(s.h / s.l);
    features[41] = log(market_s.h / market_s.l);
    // --------------------------------------------------------------------------------
    // Closing Strenght | Stock & Market
    // (close - low) / (high - low) - 1
    features[42] = (s.h > s.l) ? (s.c - s.l) / (s.h - s.l) : 0;
    features[43] = (market_s.h > market_s.l) ?
                            (market_s.c - market_s.l) / (market_s.h - market_s.l) : 0;
    // --------------------------------------------------------------------------------
    // Stock Relative VWAP 5d 10d 20d
    double  vw5  = sum(time - 4 , time + 1, 1, company, offsetof(Sample, vw)) / 5,
            vw10 = sum(time - 9 , time + 1, 1, company, offsetof(Sample, vw)) / 10,
            vw20 = sum(time - 19, time + 1, 1, company, offsetof(Sample, vw)) / 20;
    features[44] = log(s.vw / vw5);
    features[45] = log(s.vw / vw10);
    features[46] = log(s.vw / vw20);
    // --------------------------------------------------------------------------------
    // Market Relative VWAP 1d 5d 10d 20d
    double  market_vw5  = sum(time - 4 , time + 1, 1, MARKET, offsetof(Sample, vw)) / 5,
            market_vw10 = sum(time - 9 , time + 1, 1, MARKET, offsetof(Sample, vw)) / 10,
            market_vw20 = sum(time - 19, time + 1, 1, MARKET, offsetof(Sample, vw)) / 20;
    features[47] = log(market_s.vw / market_vw5);
    features[48] = log(market_s.vw/ market_vw10);
    features[49] = log(market_s.vw / market_vw20);
    // --------------------------------------------------------------------------------
    // Stock Relative Number of Transactions 5d 10d 20d
    double  n5  = sum(time - 4 , time + 1, 1, company, offsetof(Sample, n)) / 5,
            n10 = sum(time - 9 , time + 1, 1, company, offsetof(Sample, n)) / 10,
            n20 = sum(time - 19, time + 1, 1, company, offsetof(Sample, n)) / 20;
    features[50] = log(s.n / n5);
    features[51] = log(s.n / n10);
    features[52] = log(s.n / n20);
    // --------------------------------------------------------------------------------
    // Market Relative Number of Transactions 5d 10d 20d
    double  market_n5  = sum(time - 4 , time + 1, 1, MARKET, offsetof(Sample, n)) / 5,
            market_n10 = sum(time - 9 , time + 1, 1, MARKET, offsetof(Sample, n)) / 10,
            market_n20 = sum(time - 19, time + 1, 1, MARKET, offsetof(Sample, n)) / 20;
    features[53] = log(market_s.n / market_n5);
    features[54] = log(market_s.n / market_n10);
    features[55] = log(market_s.n / market_n20);
    // --------------------------------------------------------------------------------
    // Stock AVG Dollar Trade Size - Slope 5d 10d 20d
    // s.vw * s.v / s.n;
    double  d1  = s.vw * s.v / s.n,
            d5  = average_dollar_trade_size(time - 4 , time + 1, company, 0),
            d10 = average_dollar_trade_size(time - 9 , time + 1, company, 0),
            d20 = average_dollar_trade_size(time - 19, time + 1, company, 0);
    features[56] = log(d1 / d5);
    features[57] = log(d1 / d10);
    features[58] = log(d1 / d20);
    // --------------------------------------------------------------------------------
    // Market AVG Dollar Trade Size - Slope 5d 10 20d
    // market_s.vw * market_s.v / market_s.n;
    double  market_d1  = market_s.vw * market_s.v / market_s.n,
            market_d5  = average_dollar_trade_size(time - 4 , time + 1, MARKET, 0),
            market_d10 = average_dollar_trade_size(time - 9 , time + 1, MARKET, 0),
            market_d20 = average_dollar_trade_size(time - 19, time + 1, MARKET, 0);
    features[59] = features[56] - log(market_d1 / market_d5);
    features[60] = features[57] - log(market_d1 / market_d10);
    features[61] = features[58] - log(market_d1 / market_d20);
    // --------------------------------------------------------------------------------
    //                                                                                             Stock VS Market | TRENDS
    // --------------------------------------------------------------------------------------------------------------------
    // Volatility of Stock Return Against Market Return 5d 10d 20d
    features[62] = return_volatility_relative_to_market(time - 4 , time + 1, company, MARKET);
    features[63] = return_volatility_relative_to_market(time - 9 , time + 1, company, MARKET);
    features[64] = return_volatility_relative_to_market(time - 19, time + 1, company, MARKET);
    // --------------------------------------------------------------------------------------------------------------------
    // Returns Correlation to the market | Persistence 5d 10d 20d
    features[65] = return_persistence_relative_to_market(time - 4 , time + 1, company, MARKET, features[12], features[15]);
    features[66] = return_persistence_relative_to_market(time - 9 , time + 1, company, MARKET, features[13], features[16]);
    features[67] = return_persistence_relative_to_market(time - 19, time + 1, company, MARKET, features[14], features[17]);
    // --------------------------------------------------------------------------------------------------------------------
}

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

    // printf("ERROR %.16f \n", error);

    for (int i = start; i < end; i += 1) {
        updates[i] += (error * features[i - start] + LAMBDA * WEIGHTS->weights[i]) / NR_COMPANIES;
    }
    updates[WEIGHTS->len_weights + start / NR_FEATURES] += error / NR_COMPANIES;
}

void process_one_company(const Company* company, double* updates, int time) {
    
    double* features = (double*)malloc(sizeof(double) * NR_FEATURES);

    calculate_features(features, company, time);

    if (MARKET->count - time > 1) {
        int start = 0, end = NR_FEATURES;
        adjust_updates(company, updates, features, time, time + 1, start, end);
    }

    if (MARKET->count - time > 5) {
        int start = NR_FEATURES, end = NR_FEATURES * 2;
        adjust_updates(company, updates, features, time, time + 5, start, end);
    }

    if (MARKET->count - time > 10) {
        int start = NR_FEATURES * 2, end = NR_FEATURES * 3;
        adjust_updates(company, updates, features, time, time + 10, start, end);
    }

    if (MARKET->count - time > 20) {
        int start = NR_FEATURES * 3, end = NR_FEATURES * 4;
        adjust_updates(company, updates, features, time, time + 20, start, end);
    }

    free(features);
}

void print_weights() {
    printf("\n");
    for (int i = 0; i < WEIGHTS->len_weights; i += 1) {
        printf("W %d - %.16f \n", i, WEIGHTS->weights[i]);  
    }

    for (int i = 0; i < WEIGHTS->len_bias; i += 1) {
        printf("B %d - %.16f \n", i, WEIGHTS->bias[i]);
    }
}

void train(
    const Companies companies,
    const Company market,
    Weights weights
) {

    int iter = 0;

    repeat:

    // Check Error
    double* ans1 = calloc(4, sizeof(double));
    error(ans1);
    printf("\n");
    for (int i = 0; i < 4; i += 1) {
        printf("RMSE %.16f for Layer %d\n", ans1[i],i);
    }
        
    for (int time = 20; time < MARKET->count; time += 1) {
        for (int k = 0; k < EPOCHS; k += 1) {
            double* updates = (double*)calloc(WEIGHTS->len_weights + WEIGHTS->len_bias, sizeof(double));

            double alpha = ALPHA * exp((-1) * BETA * k);

            // calculate gi for each wi and save it in updates
            for (int i = 0; i < NR_COMPANIES; i += 1) {
                process_one_company(&(companies.companies[i]), updates, time);
            }

            // update wi using -= ALPHA * gi
            for (int i = 0; i < WEIGHTS->len_weights; i += 1) {
                WEIGHTS->weights[i] -= alpha * updates[i];
            }
            for (int i = 0; i < WEIGHTS->len_bias; i += 1) {
                WEIGHTS->bias[i] -= alpha * updates[WEIGHTS->len_weights + i];
            }
            
            free(updates);
        }
    }

    double* ans2 = calloc(4, sizeof(double));
    error(ans2);

    int nr = 0;
    
    for (int i = 0; i < 4; i += 1) {
        if (ans2[i] > ans1[i]) {
            nr += 1;
        }
    }

    free(ans1);
    free(ans2);
    
    if (nr == 4) {
        ALPHA /= 2;
        return;
    }

    if (iter == 20) {
        return;
    }

    iter += 1;
    
    goto repeat; 
}

void start(
    const Companies companies,
    const Company market,
    Weights weights
) {
    MARKET = &market;
    WEIGHTS = &weights;
    NR_COMPANIES = companies.len_companies;
    COMPANIES = &companies;
    train(companies, market, weights);
}
