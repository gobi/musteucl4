"""Exhaustive relative-error analysis of the 4-bit approximation
    sqrt(x^2 + y^2) ~ (a*x + b*y) / 16,  a = 0xf, b = 0x6,  0 <= y <= x <= 15.
"""
import math
from collections import Counter

A, B = 15, 6          # 0xf, 0x6  (fixed point, binary point at bit 4)
results = []
for x in range(16):
    for y in range(x + 1):
        if x == 0 and y == 0:
            continue
        exact = math.hypot(x, y)
        approx = (A * x + B * y) / 16
        rel_err = abs(approx - exact) / exact
        results.append((rel_err, x, y, approx, exact))

results.sort(reverse=True)
worst = results[0][0]
print(f"worst-case relative error = {worst*100:.3f}%  ({-math.log2(worst):.2f} bits)")
print(f"mean relative error       = {100*sum(r[0] for r in results)/len(results):.3f}%")
for e, x, y, ap, ex in results[:5]:
    print(f"  x={x:2d} y={y:2d}  approx={ap:8.4f}  exact={ex:8.4f}  err={e*100:.3f}%")

hist = Counter(int(e * 100) for e, *_ in results)
print("histogram (percent bin -> count):", sorted(hist.items()))

# Angle-only dependence: eps(theta) = |a cos(theta) + b sin(theta) - 1|
for a, b in [(0.96, 0.40), (15/16, 6/16)]:
    worst_theta = max(abs(a*math.cos(t) + b*math.sin(t) - 1)
                      for t in (i*math.pi/4/100000 for i in range(100001)))
    print(f"a={a:.4f} b={b:.4f}: max eps(theta) = {worst_theta*100:.3f}%  ({-math.log2(worst_theta):.2f} bits)")
