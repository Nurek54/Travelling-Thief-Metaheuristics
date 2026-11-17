# Porównanie metod

## Losowy [10k]

| instancja | best | worst | avg | std | ocen | czas [ms] |
|---|---:|---:|---:|---:|---:|---:|
| berlin52_n255_uncorr-similar-weights_05 | 36538.03 | -915.81 | 20308.58 | 5329.70 | 10000 | 82.3 |
| berlin52_n255_bounded-strongly-corr_05 | 105229.32 | 56998.19 | 88200.96 | 6418.18 | 10000 | 79.8 |
| kroA150_n745_uncorr-similar-weights_05 | -164565.31 | -362260.13 | -254841.54 | 27408.73 | 10000 | 285.3 |
| kroA150_n745_bounded-strongly-corr_05 | 43561.86 | -169545.15 | -44595.09 | 27562.46 | 10000 | 276.8 |
| a280_n1395_uncorr-similar-weights_05 | 373223.30 | 342050.80 | 358565.70 | 4162.70 | 10000 | 574.1 |
| a280_n1395_bounded-strongly-corr_05 | 696952.81 | 672388.12 | 685325.80 | 3038.55 | 10000 | 556.0 |
| kroA100_n495_uncorr-similar-weights_05 | -92772.64 | -278177.34 | -172055.93 | 22635.12 | 10000 | 177.2 |

## Zachłanny [n]

| instancja | best | worst | avg | std | ocen | czas [ms] |
|---|---:|---:|---:|---:|---:|---:|
| berlin52_n255_uncorr-similar-weights_05 | 58571.98 | 46763.61 | 54141.28 | 2827.05 | 52 | 0.6 |
| berlin52_n255_bounded-strongly-corr_05 | 122451.86 | 109463.54 | 116946.60 | 3542.30 | 52 | 0.6 |
| kroA150_n745_uncorr-similar-weights_05 | 167631.27 | 134871.09 | 152533.83 | 7896.34 | 150 | 9.1 |
| kroA150_n745_bounded-strongly-corr_05 | 350338.26 | 302200.79 | 326767.71 | 12109.13 | 150 | 8.9 |
| a280_n1395_uncorr-similar-weights_05 | 446832.29 | 421692.73 | 437465.46 | 4606.59 | 280 | 36.5 |
| a280_n1395_bounded-strongly-corr_05 | 745530.02 | 729636.63 | 739152.18 | 3552.81 | 280 | 33.9 |
| kroA100_n495_uncorr-similar-weights_05 | 99740.57 | 61744.91 | 82681.11 | 9039.46 | 100 | 3.3 |

## EA start [10x]

| instancja | best | worst | avg | std | ocen | czas [ms] |
|---|---:|---:|---:|---:|---:|---:|
| berlin52_n255_uncorr-similar-weights_05 | 61430.75 | 56981.28 | 59103.13 | 1371.73 | 7318 | 35.9 |
| berlin52_n255_bounded-strongly-corr_05 | 128073.18 | 123488.22 | 126781.34 | 1303.20 | 7318 | 36.2 |
| kroA150_n745_uncorr-similar-weights_05 | 32963.69 | 14660.89 | 22146.49 | 5124.21 | 7320 | 167.9 |
| kroA150_n745_bounded-strongly-corr_05 | 243579.36 | 205132.37 | 220917.50 | 10791.95 | 7320 | 161.4 |
| a280_n1395_uncorr-similar-weights_05 | 407601.42 | 397663.53 | 402153.76 | 2867.98 | 7325 | 306.7 |
| a280_n1395_bounded-strongly-corr_05 | 720318.69 | 711838.19 | 715929.00 | 2437.04 | 7325 | 311.9 |
| kroA100_n495_uncorr-similar-weights_05 | 41637.81 | 27126.89 | 33814.77 | 4675.60 | 7320 | 95.2 |

## SA start [10x]

| instancja | best | worst | avg | std | ocen | czas [ms] |
|---|---:|---:|---:|---:|---:|---:|
| berlin52_n255_uncorr-similar-weights_05 | 67206.08 | 63138.78 | 65192.26 | 1256.27 | 10101 | 74.8 |
| berlin52_n255_bounded-strongly-corr_05 | 131557.81 | 129637.39 | 130451.37 | 654.71 | 10101 | 71.2 |
| kroA150_n745_uncorr-similar-weights_05 | 134565.26 | 123072.21 | 127732.30 | 3431.57 | 10101 | 266.0 |
| kroA150_n745_bounded-strongly-corr_05 | 325146.32 | 303364.09 | 311433.33 | 5893.64 | 10101 | 260.9 |
| a280_n1395_uncorr-similar-weights_05 | 421428.37 | 415264.61 | 418217.10 | 1776.28 | 10101 | 551.1 |
| a280_n1395_bounded-strongly-corr_05 | 736614.41 | 725967.04 | 729081.61 | 3013.40 | 10101 | 541.8 |
| kroA100_n495_uncorr-similar-weights_05 | 93927.04 | 83336.65 | 89668.76 | 3756.31 | 10101 | 163.3 |

## EA dostrojony [10x]

| instancja | best | worst | avg | std | ocen | czas [ms] |
|---|---:|---:|---:|---:|---:|---:|
| berlin52_n255_uncorr-similar-weights_05 | 66098.04 | 63629.23 | 64723.68 | 827.32 | 10075 | 62.3 |
| berlin52_n255_bounded-strongly-corr_05 | 132600.53 | 130276.42 | 131887.20 | 626.58 | 10075 | 60.5 |
| kroA150_n745_uncorr-similar-weights_05 | 173642.78 | 169416.97 | 171658.36 | 1459.93 | 10075 | 202.8 |
| kroA150_n745_bounded-strongly-corr_05 | 360178.23 | 353131.35 | 356104.01 | 1998.40 | 10075 | 196.2 |
| a280_n1395_uncorr-similar-weights_05 | 457609.02 | 455280.26 | 456591.44 | 706.82 | 10075 | 438.2 |
| a280_n1395_bounded-strongly-corr_05 | 754181.53 | 751884.66 | 753250.74 | 792.80 | 10075 | 394.5 |
| kroA100_n495_uncorr-similar-weights_05 | 110611.25 | 104626.06 | 106962.05 | 1692.89 | 10075 | 125.2 |

## SA dostrojony [10x]

| instancja | best | worst | avg | std | ocen | czas [ms] |
|---|---:|---:|---:|---:|---:|---:|
| berlin52_n255_uncorr-similar-weights_05 | 66894.15 | 62639.61 | 64841.61 | 1060.67 | 10000 | 78.7 |
| berlin52_n255_bounded-strongly-corr_05 | 132883.15 | 129498.70 | 131229.73 | 1154.67 | 10000 | 73.5 |
| kroA150_n745_uncorr-similar-weights_05 | 175423.72 | 163851.55 | 168603.41 | 2890.86 | 10000 | 273.0 |
| kroA150_n745_bounded-strongly-corr_05 | 357190.67 | 348485.54 | 352119.01 | 2319.15 | 10000 | 262.8 |
| a280_n1395_uncorr-similar-weights_05 | 456401.27 | 451369.50 | 454437.04 | 1447.42 | 10000 | 547.3 |
| a280_n1395_bounded-strongly-corr_05 | 752379.04 | 747944.60 | 750761.79 | 1496.56 | 10000 | 484.1 |
| kroA100_n495_uncorr-similar-weights_05 | 108121.13 | 100830.29 | 104718.55 | 2290.95 | 10000 | 166.8 |

## Średnie wyniki (avg)

| instancja | Losowy [10k] | Zachłanny [n] | EA start [10x] | SA start [10x] | EA dostrojony [10x] | SA dostrojony [10x] |
|---|---:|---:|---:|---:|---:|---:|
| berlin52_n255_uncorr-similar-weights_05 | 20308.58 | 54141.28 | 59103.13 | 65192.26 | 64723.68 | 64841.61 |
| berlin52_n255_bounded-strongly-corr_05 | 88200.96 | 116946.60 | 126781.34 | 130451.37 | 131887.20 | 131229.73 |
| kroA150_n745_uncorr-similar-weights_05 | -254841.54 | 152533.83 | 22146.49 | 127732.30 | 171658.36 | 168603.41 |
| kroA150_n745_bounded-strongly-corr_05 | -44595.09 | 326767.71 | 220917.50 | 311433.33 | 356104.01 | 352119.01 |
| a280_n1395_uncorr-similar-weights_05 | 358565.70 | 437465.46 | 402153.76 | 418217.10 | 456591.44 | 454437.04 |
| a280_n1395_bounded-strongly-corr_05 | 685325.80 | 739152.18 | 715929.00 | 729081.61 | 753250.74 | 750761.79 |
| kroA100_n495_uncorr-similar-weights_05 | -172055.93 | 82681.11 | 33814.77 | 89668.76 | 106962.05 | 104718.55 |
