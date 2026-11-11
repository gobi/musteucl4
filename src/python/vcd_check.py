"""Parse musteucl4.vcd (SystemC trace) and verify all 136 test cases.
Each test takes 22 cycles (lead 9 + loop 4 + trail 9); clock period = 10 ps.
"""
import bisect, collections

def parse_vcd(fn):
    names, scope, data, t = {}, [], collections.defaultdict(list), 0
    for line in open(fn):
        L = line.strip()
        if L.startswith('$scope'):   scope.append(L.split()[2])
        elif L.startswith('$upscope'): scope.pop()
        elif L.startswith('$var'):
            p = L.split(); names[p[3]] = '.'.join(scope[1:] + [p[4]])
        elif L.startswith('#'):      t = int(L[1:])
        elif L.startswith('b'):
            v, idn = L[1:].split(); data[names[idn]].append((t, v))
        elif L and L[0] in '01xz' and not L.startswith('$'):
            data[names.get(L[1:], L[1:])].append((t, L[0]))
    return data

def sampler(data, sig):
    ts = [t for t, _ in data[sig]]; vs = [v for _, v in data[sig]]
    def at(tq):
        i = bisect.bisect_right(ts, tq) - 1
        return vs[i] if i >= 0 else 'x'
    return at

PERIOD, CYCLES, NTEST = 10000, 22, 136
import sys
data = parse_vcd(sys.argv[1] if len(sys.argv) > 1 else 'musteucl4.vcd')
S = {k: sampler(data, k) for k in ('x_int', 'y_int', 'u', 'v', 'z', 'z_bar')}
bits = lambda sig, t0, n: [0 if S[sig](t * PERIOD + 7500) in 'x0z' else 1 for t in range(t0, t0 + n)]
value = lambda b: sum(bit << i for i, bit in enumerate(b))

ok = {'u': 0, 'v': 0, 'z': 0}; dual_rail_bad = []
for n in range(NTEST):
    base = n * CYCLES
    x = int(S['x_int'](base * PERIOD + 125000), 2); y = int(S['y_int'](base * PERIOD + 125000), 2)
    ok['u'] += value(bits('u', base + 10, 8)) == 15 * x
    ok['v'] += value(bits('v', base + 10, 8)) == 6 * y
    ok['z'] += value(bits('z', base + 12, 9)) == 15 * x + 6 * y
    for t in range(base, base + CYCLES):
        if bits('z', t, 1)[0] == bits('z_bar', t, 1)[0]:
            dual_rail_bad.append(t)          # both rails equal = invalid dual-rail state
print(f"u=15x: {ok['u']}/{NTEST}  v=6y: {ok['v']}/{NTEST}  z=15x+6y: {ok['z']}/{NTEST}")
print("cycles with invalid dual-rail state on z:", dual_rail_bad)   # only simulation start-up (t<8)
