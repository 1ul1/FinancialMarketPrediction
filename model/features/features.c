#include "utils.h"

void calculate_raw_features(double* features, const Company* company, int time) {

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
    features[3] = features[0] - log(market_s.c / market_s5.c);
    features[4] = features[1] - log(market_s.c / market_s10.c);
    features[5] = features[2] - log(market_s.c / market_s20.c);
    // --------------------------------------------------------------------------------
    // Stock Volume 5d 10d 20d
    double  v5  = sum(time - 5 , time, 1, company, offsetof(Sample, v)) / 5,
            v10 = sum(time - 10, time, 1, company, offsetof(Sample, v)) / 10,
            v20 = sum(time - 20, time, 1, company, offsetof(Sample, v)) / 20;
    features[6] = log(s.v / v5);
    features[7] = log(s.v / v10);
    features[8] = log(s.v / v20);
    // --------------------------------------------------------------------------------
    // Market Volume 5d 10d 20d
    double  market_v5  = sum(time - 5 , time, 1, MARKET, offsetof(Sample, v)) / 5,
            market_v10 = sum(time - 10, time, 1, MARKET, offsetof(Sample, v)) / 10,
            market_v20 = sum(time - 20, time, 1, MARKET, offsetof(Sample, v)) / 20;
    features[9]  = features[6] - log(market_s.v / market_v5);
    features[10] = features[7] - log(market_s.v / market_v10);
    features[11] = features[8] - log(market_s.v / market_v20);
    // --------------------------------------------------------------------------------
    //                                                      Overall Movement Properties
    // --------------------------------------------------------------------------------
    // Stock Volatility 5d 10d 20d
    double  sd_5  = volatility(time - 4 , time + 1, company),
            sd_10 = volatility(time - 9 , time + 1, company),
            sd_20 = volatility(time - 19, time + 1, company);
    features[12] = sd_5;
    features[13] = sd_10;
    features[14] = sd_20;
    // --------------------------------------------------------------------------------
    // Market Volatility 5d 10d 20d
    double  market_sd_5  = volatility(time - 4 , time + 1, MARKET),
            market_sd_10 = volatility(time - 9 , time + 1, MARKET),
            market_sd_20 = volatility(time - 19, time + 1, MARKET);
    features[15] = (sd_5 == 0 || market_sd_5 == 0) ? 0 :
        log(sd_5 / market_sd_5);
    features[16] = (sd_10 == 0 || market_sd_10 == 0) ? 0 :
        log(sd_10 / market_sd_10);
    features[17] = (sd_20 == 0 || market_sd_20 == 0) ? 0 :
        log(sd_20 / market_sd_20);
    // --------------------------------------------------------------------------------
    // Stock Dispersion 5d 10d 20d
    features[18] = dispertion(time - 4 , time + 1, company);
    features[19] = dispertion(time - 9 , time + 1, company);
    features[20] = dispertion(time - 19, time + 1, company);
    // --------------------------------------------------------------------------------
    // Market Dispersion 5d 10d 20d
    features[21] = log(features[18] + 1) - log(dispertion(time - 4 , time + 1, MARKET) + 1);
    features[22] = log(features[19] + 1) - log(dispertion(time - 9 , time + 1, MARKET) + 1);
    features[23] = log(features[20] + 1) - log(dispertion(time - 19, time + 1, MARKET) + 1);
    // --------------------------------------------------------------------------------
    // Stock Stability 5d 10d 20d
    features[24] = stability(time - 4 , time + 1, company);
    features[25] = stability(time - 9 , time + 1, company);
    features[26] = stability(time - 19, time + 1, company);
    // --------------------------------------------------------------------------------
    // Market Stability 5d 10d 20d
    features[27] = log(features[24] + 1) - log(stability(time - 4 , time + 1, MARKET) + 1);
    features[28] = log(features[25] + 1) - log(stability(time - 9 , time + 1, MARKET) + 1);
    features[29] = log(features[26] + 1) - log(stability(time - 19, time + 1, MARKET) + 1);
    // --------------------------------------------------------------------------------
    // Stock Persistence 10d 20d trend
    features[30] = persistence(time - 9 , time + 1, company);
    features[31] = persistence(time - 19, time + 1, company);
    features[32] = features[2] * features[31];
    // --------------------------------------------------------------------------------
    // Market Persistence 10d 20d trend
    features[33] = features[30] - persistence(time - 9 , time + 1, MARKET);
    features[34] = features[31] - persistence(time - 19, time + 1, MARKET);
    features[35] = features[5] * features[34];
    // --------------------------------------------------------------------------------
    //                                                                   Today's Action
    // --------------------------------------------------------------------------------
    // Gap | Stock & Market
    // ln(Current day Open Price / Previous day Close)
    features[36] = log(s.o / s1.c);
    features[37] = features[36] - log(market_s.o / market_s1.c);
    // --------------------------------------------------------------------------------
    // Intraday Move | Stock & Market
    // ln(Close / Open)
    features[38] = log(s.c / s.o);
    features[39] = features[38] - log(market_s.c / market_s.o);
    // --------------------------------------------------------------------------------
    // Range | Stock & Market
    // ln(high / low)
    features[40] = log(s.h / s.l);
    features[41] = features[40] - log(market_s.h / market_s.l);
    // --------------------------------------------------------------------------------
    // Closing Strenght | Stock & Market
    // (close - low) / (high - low) - 1
    features[42] = (s.h > s.l) ? (s.c - s.l) / (s.h - s.l) : 0;
    features[43] = (market_s.h == market_s.l) ? 0 :
        features[42] - (market_s.c - market_s.l) / (market_s.h - market_s.l);
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
    features[47] = features[44] - log(market_s.vw / market_vw5);
    features[48] = features[45] - log(market_s.vw/ market_vw10);
    features[49] = features[46] - log(market_s.vw / market_vw20);
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
    features[53] = features[50] - log(market_s.n / market_n5);
    features[54] = features[51] - log(market_s.n / market_n10);
    features[55] = features[52] - log(market_s.n / market_n20);
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
    // Returns Correlation to the market | Persistence 10d 20d trend
    features[65] = (sd_10 * market_sd_10 == 0) ? 0 :
        return_covariance_to_market(time - 9 , time + 1, company, MARKET) / (sd_10 * market_sd_10);
    features[66] = (sd_20 * market_sd_20 == 0) ? 0 :
        return_covariance_to_market(time - 19, time + 1, company, MARKET) / (sd_20 * market_sd_20);
    features[67] = features[65] - features[66];
    // --------------------------------------------------------------------------------------------------------------------
    // Beta Market 10d 20d (completes the previous persistence cause it tells quantity)
    // It is rebundant but cause of Ridge, multilinearity is handled; also the model would find it hardly otherwise
    features[68] = (market_sd_10 == 0) ? 0 :
        return_covariance_to_market(time - 9 , time, company, MARKET) / (pow(volatility(time - 9 , time, MARKET), 2));
    features[69] = (market_sd_20 == 0) ? 0 :
        return_covariance_to_market(time - 19, time, company, MARKET) / (pow(volatility(time - 19, time, MARKET), 2));
    // --------------------------------------------------------------------------------------------------------------------
    // Residual 10d 20d
    double
            beta_10 = features[68],
            beta_20 = features[69],
            alpha_10 = log(s1.c / s10.c ) / 9  - beta_10 * log(market_s1.c / market_s10.c ) / 9,
            alpha_20 = log(s1.c / s20.c ) / 19 - beta_20 * log(market_s1.c / market_s20.c ) / 19;
    features[70] = log(s.c / s1.c) - alpha_10 - beta_10 * log(market_s.c / market_s1.c);
    features[71] = log(s.c / s1.c) - alpha_20 - beta_20 * log(market_s.c / market_s1.c);
}

void calculate_features(double** features) {
    for (int time = 20; time < MARKET->count; time += 1) {
        calculate_raw_features(features[time], COMPANY, time);
        for (int f = 0; f < NR_FEATURES; f += 1) {
            features[time][f] = (WEIGHTS->standard_deviations[f] < 1e-12) ? 0.0
                : (features[time][f] - WEIGHTS->means[f]) / WEIGHTS->standard_deviations[f];
        }
    }
}
