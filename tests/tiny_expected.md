# Test reczny: tiny.ttp

Miasta (od 0): 0=(0,0), 1=(3,0), 2=(3,4), 3=(0,4)
Przedmioty (od 0): p0: zysk 10, waga 2, miasto 1 | p1: zysk 20, waga 3, miasto 2 | p2: zysk 5, waga 4, miasto 3
W = 5, vmin = 0.1, vmax = 1. Trasa: 0 -> 1 -> 2 -> 3 -> 0

Odleglosci: d(0,1)=3, d(1,2)=4, d(2,3)=3, d(3,0)=4

Pakowanie (droga do konca liczona od miasta przedmiotu):
- p0: 10 / (2 * 11) = 0.4545
- p1: 20 / (3 * 7)  = 0.9524
- p2:  5 / (4 * 4)  = 0.3125
Kolejnosc: p1 (waga 3), p0 (waga 2, razem 5, miesci sie), p2 (razem 9, nie miesci sie)
Wybrane: p0, p1

Podroz:
- 0 -> 1: Wc=0, v=1.00, t = 3 / 1.00 = 3.00   | zbieramy p0: Wc=2, zysk=10
- 1 -> 2: Wc=2, v=0.64, t = 4 / 0.64 = 6.25   | zbieramy p1: Wc=5, zysk=30
- 2 -> 3: Wc=5, v=0.10, t = 3 / 0.10 = 30.00  | p2 nie wybrany
- 3 -> 0: Wc=5, v=0.10, t = 4 / 0.10 = 40.00

Czas = 79.25, zysk = 30, G = 30 - 79.25 = -49.25
