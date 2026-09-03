# The Stock Prediction Model behind [byebility.com](https://predictions.byebility.com)

- **Ridge regression model** for stock return forecasting written in raw C, no external ML framework or numerical library anywhere in the loop.
- Data is collected and validated by a self-written scraper.
- All features are centered and scaled.

## Overview

- Market reference: SPY
- 50+ features per sample
- 4 independent predictions (1/5/10/20 day returns)
- Weights are pretrained across a company universe, then fine-tuned and calibrated per ticker before each forecast
- True out-of-sample skill is measured against a zero-return RMSE, on unseen stocks during a time window excluded from training

## Output

For a given ticker and date, each of the 4 horizons returns:

- **Starting price**: same for all horizons
- **Expected return**: the fine-tuned prediction, bias corrected
- **Expected price**: starting price × exp(expected return)
- **Residual bias**: the ticker's average historical miss for this horizon, converted to dollars
- **Residual standard deviation**: the spread of residuals after the bias correction, converted to dollars
- **Forecast strength**: z-score of how unusual this forecast is relative to the ticker's own noise (computed before the dollar conversion)

## Features & Mathematics

72 features per sample, computed for both the stock and the market (SPY).

Notation: daily log return $r_s=\ln(C_s/C_{s-1})$, window $W_L(t)=\{t-L+1,\dots,t\}$ with $L\in\{5,10,20\}$ (for my horizons), $S$ = stock, $M$ = market.

- **Momentum**: log return over the lookback window

- **Relative volume**: today's volume vs. its rolling average

- **Volatility**: standard deviation of daily log returns

  $$\sqrt{\frac{1}{L}\sum\big(r_s-\bar r_L\big)^2}$$

- **Dispersion**: how spread out the price path is around its own mean, i.e. the standard deviation of closes

  $$\sqrt{\frac{1}{L}\sum\big(C_s-\bar C_L\big)^2}$$

- **Stability**: how much day-to-day *changes* in returns fluctuate, i.e. the RMS of the change in returns

  $$\sqrt{\frac{1}{L-1}\sum\big(r_s-r_{s-1}\big)^2}$$

- **Persistence**: autocorrelation of returns (continuation vs. reversion), the lag-1 Pearson correlation

  $$\frac{\frac{1}{L-1}\sum\big(r_s-\bar X\big)\big(r_{s+1}-\bar Y\big)}{\sigma_X\,\sigma_Y}$$

- **Gap, intraday move, range, closing strength**: same-day price action

- **Relative VWAP, relative transaction count, average trade-size slope**

- **Relative volatility** (stock vs. market): stock returns dispersed around the *market's* mean return

  $$\sqrt{\frac{1}{L}\sum\big(r-\bar r\big)^2}$$

- **Market correlation** (stock vs. market): Pearson correlation between stock and market returns

  $$\frac{\frac{1}{L}\sum\big(r_s-\bar r_s\big)\big(r_m-\bar r_m\big)}{\sigma_s\,\sigma_m}$$

- **Market beta**: stock sensitivity to market returns

  $$\frac{\frac{1}{L}\sum\big(r_s-\bar r_s\big)\big(r_m-\bar r_m\big)}{\sigma_m^2}$$

- **Residual return**: stock return unexplained by the market

  $$r_{s,t}-\alpha-\beta r_{m,t}\qquad \text{with}\qquad \alpha=\bar r_s-\beta\bar r_m$$

- **Centering and scaling** | uses the training set mean and standard deviation:

$$z=\frac{x-\bar x}{\sigma_x}$$

## Training universe

One JSON file per company, plus a single SPY file used as the market reference. All are aligned in time and for each day they reveal `v`, `vw`, `o`, `c`, `h`, `l`, `t`, `n`.

Every sample is validated, any faulty data (either missing timeframes or null fields) does not enter training.

![Price distribution](plots/stock_universe_plots/price_distribution.png)

![Volume distribution](plots/stock_universe_plots/volume_distribution.png)

![Volatility distribution](plots/stock_universe_plots/volatility_distribution.png)

![Volatility vs liquidity](plots/stock_universe_plots/volatility_vs_liquidity.png)

## Model

Each horizon is an independent ridge regression on log-returns. All 4 heads share the same 72 input features:

| Horizon | Look-ahead | Weight slice | Bias index |
|---|---|---|---|
| 1 day  | `t + 1`  | `weights[0:72]`    | `bias[0]` |
| 5 day  | `t + 5`  | `weights[72:144]`  | `bias[1]` |
| 10 day | `t + 10` | `weights[144:216]` | `bias[2]` |
| 20 day | `t + 20` | `weights[216:288]` | `bias[3]` |

All parameters flow through both training stages below: first fit across the whole universe, then finetune to one company at a time.

## Training - 2 stages

### Stage 1 | Cross-sectional pretraining (`training.c`)

Walks forward through calendar time across the *entire* company universe at once:

1. At each trading day, compute the prediction error for every company in the universe and average the gradient across all of them: a full batch over the cross-section, to minimize the effects of constant Market features among all companies on that day.
2. After a full walk through, recompute RMSE on all 4 horizons. If every horizon got strictly worse, halve the learning rate, otherwise repeat.

They are also constantly statically saved on disk every couple of cycles during training.

### Centering and scaling

All features are now centered and scaled using each feature's row's mean and standard deviation. Without it training proved to be too slow, as below is the graph of the weights' evolution over a couple hours of training...

![Weight heatmap](plots/weight_heatmap.png)

### VS

Current IQR

![Weight IQR](plots/iqr_indexed.png)

### Stage 2 | Per-ticker fine-tuning & calibration (`model.c`)

Every time a forecast is requested for a ticker, the pretrained weights are adapted specifically to that company before predicting, using ridge regression once again.

If the most recent bar in the data belongs to today's still-open session, it's excluded from both fine-tuning and calibration, so the model never trains or predicts on a price that hasn't closed yet.
