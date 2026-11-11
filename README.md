# musteucl4 — Approximate Euclidean distance in spiking neural P systems

SystemC model, verification scripts and simulation traces for

> O. Agvaan, T.-O. Ishdorj, G. Cichon, et al. *Optimal approximate computation of
> Euclidean distance in spiking neural P systems framework.*
> Scientific Reports 15, 18047 (2025). https://doi.org/10.1038/s41598-025-02793-3

The circuit (MUST-Euclidean, `musteucl4`) approximates
`sqrt(x^2 + y^2) ≈ (15·max + 6·min)/16` for 4-bit inputs using three
MUST-Adders built from HP/LP spiking neurons (42 neurons, dual-rail,
one neuron step = one clock cycle). A second variant (`musteucl4_b`,
`8·max + 3·min`, 32 neurons, no subtractor) is included.

## Layout

```
src/systemc/   SystemC 2.3 / 3.0 model
  neuron.h         HP / LP basic neuron (SN P semantics, synchronous)
  must_adder.h     MUST-Adder: 10 neurons, 2-cycle latency
  musteucl4.h      variant A  (15x + 6y), 42 neurons
  musteucl4_b.h    variant B  ( 8x + 3y), 32 neurons
  testbench.cpp    136 input pairs (x ≥ y), checks u, v, z bit by bit, writes VCD
  testbench_b.cpp  same for variant B
  Makefile
  compare_vcd.py   bit-exact comparison of two VCD files
src/python/    independent bit-level checks (no SystemC needed)
  musteucl4_prefix_check.py   adder/prefix model, variants A, A', B — 1088 cases
  vcd_check.py                decodes u, v, z from a VCD and checks all 136 tests
  exhaustive_error.py         error analysis of the a·max + b·min approximation
vcd/           reference traces produced by this model
  musteucl4.vcd      variant A, 136 tests × 22 cycles — bit-identical (after the
                     12 start-up cycles) to the trace used in the paper
  musteucl4_b.vcd    variant B, 136 tests × 14 cycles
```

## Build and run

Linux (Debian/Ubuntu):
```sh
sudo apt install libsystemc-dev
cd src/systemc && make && ./musteucl4 && ./musteucl4_b
```

macOS (Homebrew):
```sh
brew install systemc
cd src/systemc && make SYSTEMC=$(brew --prefix systemc)
./musteucl4        # -> "checked 136 tests, 0 errors", writes musteucl4.vcd
./musteucl4_b      # -> "variant B: checked 136 tests, 0 errors"
```

Compare a freshly written trace with the reference in `vcd/` (the first
12 cycles are start-up and are skipped):
```sh
python3 compare_vcd.py ../../vcd/musteucl4.vcd musteucl4.vcd   # RESULT: IDENTICAL
```

Bit-level checks without SystemC:
```sh
python3 src/python/musteucl4_prefix_check.py
python3 src/python/vcd_check.py vcd/musteucl4.vcd
```

## Viewing the traces

Any VCD viewer works. GTKWave:

* Linux: `sudo apt install gtkwave`
* macOS 14+: the Homebrew cask is gone; build from the maintainer's tap
  `brew install --HEAD randomplum/gtkwave/gtkwave`, or use
  [Surfer](https://surfer-project.org).
* Windows: https://sourceforge.net/projects/gtkwave/files/ (3.3.x win64 zip),
  add `gtkwave\bin` to `PATH`, then `gtkwave vcd\musteucl4.vcd`.

In `musteucl4.vcd` the tests run in the order (x, y) = (0,0), (1,0), (1,1), (2,0), …
(x ≥ y, 136 pairs); test *i* starts at 220·i ns; `u = 15x` appears on cycles 10–17, `v = 6y` on 10–17 and
`z = u + v` on 12–20 of each test, LSB first.

## Citation

If you use this code, please cite the Scientific Reports paper above.
