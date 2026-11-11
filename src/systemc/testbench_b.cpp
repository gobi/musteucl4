// testbench_b.cpp — Хувилбар B (хасагчгүй, w' = 8x + 3y) бүрэн тест
//   lead (2): x <- 00, y <- 00;  loop (4): x0..x3, y0..y3;  trail (8): 0
// Гаралт musteucl4_b.vcd
#include "musteucl4_b.h"
#include <cstdio>

static const int LEAD = 2, LOOP = 4, TRAIL = 8, CYCLES = LEAD + LOOP + TRAIL;

SC_MODULE(TestbenchB) {
    sc_in<bool>     clk;
    sc_signal<bool> x, x_bar, y, y_bar;
    sc_signal<int>  x_int, y_int;
    sc_signal<bool> lead, loop, trail;
    MustEucl4B*     dut;
    int cycle = 0, test = 0, cur_x = 0, cur_y = 0, errors = 0, checked = 0;
    unsigned long acc_v = 0, acc_z = 0;
    static const int T_V = 3, T_Z = 5;      // v (3y) LSB цикл 3-т, z (8x+3y) LSB цикл 5-д

    void stimulus() {
        int n = 0;
        for (int xx = 0; xx < 16; ++xx) for (int yy = 0; yy <= xx; ++yy, ++n) if (n == test) { cur_x = xx; cur_y = yy; }
        int bx = 0, by = 0;
        if (cycle >= LEAD && cycle < LEAD + LOOP) { int i = cycle - LEAD; bx = (cur_x >> i) & 1; by = (cur_y >> i) & 1; }
        x.write(bx); x_bar.write(!bx); y.write(by); y_bar.write(!by);
        x_int.write(cur_x); y_int.write(cur_y);
        lead.write(cycle < LEAD); loop.write(cycle >= LEAD && cycle < LEAD + LOOP); trail.write(cycle >= LEAD + LOOP);
    }
    void check() {
        if (cycle >= T_V && cycle < T_V + 8) acc_v |= (unsigned long)dut->v.read() << (cycle - T_V);
        if (cycle >= T_Z && cycle < T_Z + 8) acc_z |= (unsigned long)dut->z.read() << (cycle - T_Z);
        if (cycle == CYCLES - 1) {
            unsigned long ev = 3UL * cur_y, ez = 8UL * cur_x + ev;
            if (acc_v != ev || acc_z != ez) { ++errors; printf("FAIL x=%2d y=%2d  v=%lu (exp %lu)  z=%lu (exp %lu)\n", cur_x, cur_y, acc_v, ev, acc_z, ez); }
            ++checked; acc_v = acc_z = 0;
        }
    }
    void tick() {
        check();
        if (++cycle == CYCLES) { cycle = 0; ++test; }
        if (test == 136) { printf("variant B: checked %d tests, %d errors\n", checked, errors); sc_stop(); return; }
        stimulus();
    }
    SC_CTOR(TestbenchB)
        : x("x"), x_bar("x_bar"), y("y"), y_bar("y_bar"), x_int("x_int"), y_int("y_int"), lead("lead"), loop("loop"), trail("trail")
    {
        dut = new MustEucl4B("musteucl4_b");
        dut->clk(clk); dut->x(x); dut->x_bar(x_bar); dut->y(y); dut->y_bar(y_bar);
        SC_METHOD(tick); sensitive << clk.neg(); dont_initialize();
        stimulus();
    }
};

int sc_main(int, char**) {
    sc_clock clk("clock", 10, SC_NS);
    TestbenchB tb("tb"); tb.clk(clk);
    sc_trace_file* tf = sc_create_vcd_trace_file("musteucl4_b");
    tf->set_time_unit(1, SC_PS);
    sc_trace(tf, clk, "clock");
    sc_trace(tf, tb.x, "x"); sc_trace(tf, tb.x_bar, "x_bar");
    sc_trace(tf, tb.dut->x1, "x1"); sc_trace(tf, tb.dut->x2, "x2"); sc_trace(tf, tb.dut->x3, "x3"); sc_trace(tf, tb.dut->x4, "x4"); sc_trace(tf, tb.dut->x5, "x5");
    sc_trace(tf, tb.y, "y"); sc_trace(tf, tb.y_bar, "y_bar"); sc_trace(tf, tb.dut->y1, "y1");
    sc_trace(tf, tb.dut->v, "v"); sc_trace(tf, tb.dut->v_bar, "v_bar");
    sc_trace(tf, tb.dut->z, "z"); sc_trace(tf, tb.dut->z_bar, "z_bar");
    sc_trace(tf, tb.x_int, "x_int"); sc_trace(tf, tb.y_int, "y_int");
    sc_trace(tf, tb.lead, "lead"); sc_trace(tf, tb.loop, "loop"); sc_trace(tf, tb.trail, "trail");
    sc_trace(tf, tb.dut->addy->c, "addy.c"); sc_trace(tf, tb.dut->addz->c, "addz.c");
    sc_start();
    sc_close_vcd_trace_file(tf);
    return 0;
}
