"""Bit-level model of MUST-Euclidean (musteucl4): three serial full adders.
Verifies that the leading sequences 000010000 (x) and 000000000 (y) initialise
the carries correctly for every input 0 <= y <= x <= 15 and every initial carry state.
"""

def serial_adder(A, B, c0):
    """Serial full adder. Returns sum bits S(t) and incoming carries C(t)."""
    c, S, C = c0, [], []
    for a, b in zip(A, B):
        C.append(c)
        S.append(a ^ b ^ c)
        c = (a & b) | ((a ^ b) & c)
    return S, C

def musteucl4(x, y, c0x=0, c0y=0, c0z=0, T=24):
    xb = [(x >> i) & 1 for i in range(4)]          # LSB first
    yb = [(y >> i) & 1 for i in range(4)]
    sx = [0, 0, 0, 0, 1, 0, 0, 0, 0] + xb           # prefix 000010000, then x
    sy = [0] * 9 + yb                               # prefix 000000000, then y
    sx += [0] * (T - len(sx)); sy += [0] * (T - len(sy))
    delay = lambda s, k: [0] * k + s[:T - k]
    Ax, Bx = delay(sx, 4), [1 - v for v in sx]      # x<<4 and NOT x
    Ay, By = delay(sy, 1), delay(sy, 2)             # y<<1 and y<<2
    Sx, _ = serial_adder(Ax, Bx, c0x)
    Sy, _ = serial_adder(Ay, By, c0y)
    Sz, _ = serial_adder(Sx, Sy, c0z)
    return sum(Sz[9 + i] << i for i in range(9))    # w_0..w_8 appear at t = 9..17

def variant(x, y, kind, c0=(0, 0, 0), T=24):
    """kind: 'A' (y1,y2; prefixes 9/9), 'Aprime' (y2,y3; prefixes 9/8), 'B' (8x+3y, no subtractor; prefixes 2/2)."""
    xb = [(x >> i) & 1 for i in range(4)]; yb = [(y >> i) & 1 for i in range(4)]
    delay = lambda s, k: [0] * k + s[:T - k]
    if kind in ('A', 'Aprime'):
        sx = [0, 0, 0, 0, 1, 0, 0, 0, 0] + xb
        sy = ([0] * 9 if kind == 'A' else [0] * 8) + yb
        sx += [0] * (T - len(sx)); sy += [0] * (T - len(sy))
        Sx, _ = serial_adder(delay(sx, 4), [1 - v for v in sx], c0[0])
        Sy, _ = serial_adder(*((delay(sy, 1), delay(sy, 2)) if kind == 'A' else (delay(sy, 2), delay(sy, 3))), c0[1])
        Sz, _ = serial_adder(Sx, Sy, c0[2])
        return sum(Sz[9 + i] << i for i in range(9)), 15 * x + 6 * y
    sx = [0, 0] + xb; sy = [0, 0] + yb
    sx += [0] * (T - len(sx)); sy += [0] * (T - len(sy))
    Sy, _ = serial_adder(delay(sy, 1), sy, c0[1])          # 3y = 2y + y
    Sz, _ = serial_adder(delay(sx, 3), Sy, c0[2])          # 8x + 3y
    return sum(Sz[2 + i] << i for i in range(8)), 8 * x + 3 * y

if __name__ == "__main__":
    for kind in ('A', 'Aprime', 'B'):
        ok = sum(variant(x, y, kind, c)[0] == variant(x, y, kind, c)[1]
                 for x in range(16) for y in range(x + 1)
                 for c in [(a, b, d) for a in (0, 1) for b in (0, 1) for d in (0, 1)])
        print(f"variant {kind}: {ok}/1088 correct")
    ok = sum(musteucl4(x, y, *c) == 15 * x + 6 * y
             for x in range(16) for y in range(x + 1)
             for c in [(a, b, d) for a in (0, 1) for b in (0, 1) for d in (0, 1)])
    print(f"musteucl4 (variant A, full model): {ok}/1088 correct")
