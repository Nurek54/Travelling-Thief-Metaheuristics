# Porównanie metod

## Losowy [10k]

| instancja | best | worst | avg | std | ocen | czas [ms] |
|---|---:|---:|---:|---:|---:|---:|
| berlin52_n255_uncorr-similar-weights_05 | 38732.36 | -4129.85 | 20256.39 | 5403.90 | 10000 | 75.1 |
| berlin52_n255_bounded-strongly-corr_05 | 108991.54 | 56290.60 | 88148.73 | 6489.00 | 10000 | 71.5 |
| kroA150_n745_uncorr-similar-weights_05 | -153405.30 | -378541.55 | -254662.58 | 27693.21 | 10000 | 248.8 |
| kroA150_n745_bounded-strongly-corr_05 | 50852.56 | -159261.21 | -44492.37 | 27870.22 | 10000 | 243.4 |
| a280_n1395_uncorr-similar-weights_05 | 373834.21 | 342585.73 | 358514.01 | 4196.33 | 10000 | 555.6 |
| a280_n1395_bounded-strongly-corr_05 | 695361.31 | 671868.46 | 685295.16 | 3000.45 | 10000 | 507.7 |
| kroA100_n495_uncorr-similar-weights_05 | -86579.98 | -261159.38 | -172136.19 | 22727.89 | 10000 | 164.4 |

## Zachłanny [n]

| instancja | best | worst | avg | std | ocen | czas [ms] |
|---|---:|---:|---:|---:|---:|---:|
| berlin52_n255_uncorr-similar-weights_05 | 58571.98 | 46763.61 | 54141.28 | 2827.05 | 52 | 0.6 |
| berlin52_n255_bounded-strongly-corr_05 | 122451.86 | 109463.54 | 116946.60 | 3542.30 | 52 | 0.6 |
| kroA150_n745_uncorr-similar-weights_05 | 167631.27 | 134871.09 | 152533.83 | 7896.34 | 150 | 9.7 |
| kroA150_n745_bounded-strongly-corr_05 | 350338.26 | 302200.79 | 326767.71 | 12109.13 | 150 | 8.5 |
| a280_n1395_uncorr-similar-weights_05 | 446832.29 | 421692.73 | 437465.46 | 4606.59 | 280 | 38.5 |
| a280_n1395_bounded-strongly-corr_05 | 745530.02 | 729636.63 | 739152.18 | 3552.81 | 280 | 44.5 |
| kroA100_n495_uncorr-similar-weights_05 | 99740.57 | 61744.91 | 82681.11 | 9039.46 | 100 | 3.3 |

## EA start [10x]

| instancja | best | worst | avg | std | ocen | czas [ms] |
|---|---:|---:|---:|---:|---:|---:|
| berlin52_n255_uncorr-similar-weights_05 | 63069.48 | 58001.18 | 60246.68 | 1581.72 | 7332 | 34.4 |
| berlin52_n255_bounded-strongly-corr_05 | 127477.73 | 125130.25 | 126543.24 | 876.62 | 7332 | 30.1 |
| kroA150_n745_uncorr-similar-weights_05 | 39949.03 | 8034.44 | 24957.59 | 8670.89 | 7333 | 140.2 |
| kroA150_n745_bounded-strongly-corr_05 | 239355.58 | 201104.75 | 222660.85 | 12229.69 | 7333 | 142.7 |
| a280_n1395_uncorr-similar-weights_05 | 404955.13 | 394146.35 | 402035.21 | 3467.68 | 7330 | 275.3 |
| a280_n1395_bounded-strongly-corr_05 | 721133.11 | 711753.54 | 715048.25 | 2624.30 | 7330 | 268.8 |
| kroA100_n495_uncorr-similar-weights_05 | 44432.54 | 24767.58 | 35213.14 | 6391.28 | 7333 | 83.4 |

## SA start [10x]

| instancja | best | worst | avg | std | ocen | czas [ms] |
|---|---:|---:|---:|---:|---:|---:|
| berlin52_n255_uncorr-similar-weights_05 | 66428.28 | 63147.39 | 65006.39 | 867.19 | 10101 | 68.3 |
| berlin52_n255_bounded-strongly-corr_05 | 132338.05 | 129333.45 | 130974.00 | 838.29 | 10101 | 62.5 |
| kroA150_n745_uncorr-similar-weights_05 | 130578.91 | 117850.19 | 125379.23 | 3882.83 | 10101 | 234.6 |
| kroA150_n745_bounded-strongly-corr_05 | 324242.06 | 306868.91 | 316719.66 | 4997.09 | 10101 | 227.8 |
| a280_n1395_uncorr-similar-weights_05 | 422589.86 | 416148.48 | 418907.46 | 2174.66 | 10101 | 503.3 |
| a280_n1395_bounded-strongly-corr_05 | 733466.17 | 725145.04 | 728867.24 | 2155.20 | 10101 | 487.7 |
| kroA100_n495_uncorr-similar-weights_05 | 97651.59 | 84135.05 | 90750.73 | 3827.78 | 10101 | 147.2 |

## EA dostrojony [10x]

| instancja | best | worst | avg | std | ocen | czas [ms] |
|---|---:|---:|---:|---:|---:|---:|
| berlin52_n255_uncorr-similar-weights_05 | 66905.72 | 62820.63 | 65082.81 | 1207.22 | 10075 | 51.0 |
| berlin52_n255_bounded-strongly-corr_05 | 132458.11 | 130957.75 | 131827.70 | 502.21 | 10075 | 47.3 |
| kroA150_n745_uncorr-similar-weights_05 | 173460.33 | 168519.68 | 171451.38 | 1513.12 | 10075 | 165.6 |
| kroA150_n745_bounded-strongly-corr_05 | 358166.83 | 351946.42 | 354693.69 | 1728.24 | 10075 | 158.9 |
| a280_n1395_uncorr-similar-weights_05 | 458769.11 | 454923.84 | 457172.74 | 1260.71 | 10075 | 380.6 |
| a280_n1395_bounded-strongly-corr_05 | 754931.14 | 751409.91 | 753488.69 | 880.20 | 10075 | 339.0 |
| kroA100_n495_uncorr-similar-weights_05 | 106971.13 | 104236.48 | 105911.00 | 846.29 | 10075 | 105.3 |

## SA dostrojony [10x]

| instancja | best | worst | avg | std | ocen | czas [ms] |
|---|---:|---:|---:|---:|---:|---:|
| berlin52_n255_uncorr-similar-weights_05 | 67416.22 | 62145.22 | 65219.00 | 1580.11 | 10000 | 67.1 |
| berlin52_n255_bounded-strongly-corr_05 | 133157.60 | 126331.24 | 131066.51 | 1794.56 | 10000 | 60.5 |
| kroA150_n745_uncorr-similar-weights_05 | 172560.74 | 164775.78 | 169312.60 | 2156.41 | 10000 | 237.5 |
| kroA150_n745_bounded-strongly-corr_05 | 357442.25 | 347277.50 | 352407.06 | 3299.58 | 10000 | 228.7 |
| a280_n1395_uncorr-similar-weights_05 | 456760.98 | 452471.09 | 454593.85 | 1323.15 | 10000 | 499.1 |
| a280_n1395_bounded-strongly-corr_05 | 753321.53 | 748991.03 | 750771.31 | 1312.82 | 10000 | 432.0 |
| kroA100_n495_uncorr-similar-weights_05 | 109044.48 | 104971.10 | 106474.55 | 1443.88 | 10000 | 147.7 |

## Średnie wyniki (avg)

| instancja | Losowy [10k] | Zachłanny [n] | EA start [10x] | SA start [10x] | EA dostrojony [10x] | SA dostrojony [10x] |
|---|---:|---:|---:|---:|---:|---:|
| berlin52_n255_uncorr-similar-weights_05 | 20256.39 | 54141.28 | 60246.68 | 65006.39 | 65082.81 | 65219.00 |
| berlin52_n255_bounded-strongly-corr_05 | 88148.73 | 116946.60 | 126543.24 | 130974.00 | 131827.70 | 131066.51 |
| kroA150_n745_uncorr-similar-weights_05 | -254662.58 | 152533.83 | 24957.59 | 125379.23 | 171451.38 | 169312.60 |
| kroA150_n745_bounded-strongly-corr_05 | -44492.37 | 326767.71 | 222660.85 | 316719.66 | 354693.69 | 352407.06 |
| a280_n1395_uncorr-similar-weights_05 | 358514.01 | 437465.46 | 402035.21 | 418907.46 | 457172.74 | 454593.85 |
| a280_n1395_bounded-strongly-corr_05 | 685295.16 | 739152.18 | 715048.25 | 728867.24 | 753488.69 | 750771.31 |
| kroA100_n495_uncorr-similar-weights_05 | -172136.19 | 82681.11 | 35213.14 | 90750.73 | 105911.00 | 106474.55 |
