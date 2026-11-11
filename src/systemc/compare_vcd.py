"""compare_vcd.py — Хоёр VCD бичлэгийг цикл бүрээр, дохио бүрээр тулгана.

Хэрэглээ:  python3 compare_vcd.py <эталон.vcd> <шинэ.vcd> [--skip N]
  --skip N : эхний N циклийг (симуляцийн эхлэлийн тодорхойгүй төлөв) тооцохгүй (анхдагч 12)

Анхны бичлэгт дохио эерэг фронтод, шинэ тестбенчид оролт сөрөг фронтод өөрчлөгддөг тул
эталоныг фронтоос +7.5 ns, шинийг +2.5 ns цэгт түүвэрлэнэ (цаг 10 ns).
"""
import sys, bisect, collections

PERIOD = 10000  # ps

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
            if L[1:] in names: data[names[L[1:]]].append((t, L[0]))
    return data

def sampler(data, sig, offset):
    ts = [t for t, _ in data[sig]]; vs = [v for _, v in data[sig]]
    def at(c):
        i = bisect.bisect_right(ts, c * PERIOD + offset) - 1
        return vs[i] if i >= 0 else 'x'
    return at

def main():
    args = [a for a in sys.argv[1:] if not a.startswith('--')]
    skip = int(sys.argv[sys.argv.index('--skip') + 1]) if '--skip' in sys.argv else 12
    ref, new = parse_vcd(args[0]), parse_vcd(args[1])
    last = max(t for ev in ref.values() for t, _ in ev)
    ncyc = last // PERIOD + 1
    common = sorted(set(ref) & set(new) - {'clock'})
    total_bad = 0
    print(f"{'signal':12s} {'mismatch':>9s}  (of {ncyc} cycles, first {skip} skipped)")
    for sig in common:
        fr, fn_ = sampler(ref, sig, 7500), sampler(new, sig, 2500)
        bad = [c for c in range(skip, ncyc) if fr(c) != fn_(c)]
        total_bad += len(bad)
        print(f"{sig:12s} {len(bad):9d}  {bad[:5] if bad else ''}")
    print("RESULT:", "IDENTICAL" if total_bad == 0 else f"{total_bad} mismatching samples")

if __name__ == '__main__':
    main()
