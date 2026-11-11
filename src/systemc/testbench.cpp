// testbench.cpp — MUST-Euclidean (musteucl4) бүрэн тест
//
// 0 <= y <= x <= 15 бүх 136 хосыг дарааллаар, хос бүрт 22 цикл:
//   lead  (9 цикл): x <- 000010000,  y <- 000000000   (урьдал дараалал)
//   loop  (4 цикл): x <- x0..x3,     y <- y0..y3      (LSB эхэлж)
//   trail (9 цикл): x <- 0,          y <- 0
// Давхар шугамт оролт: x_bar = !x, y_bar = !y.
// Бүх дохиог musteucl4.vcd файлд бичнэ (цаг 10 ns = 1 цикл).
//
// Build:  make            Run: ./musteucl4      View: gtkwave musteucl4.vcd
#include "musteucl4.h"
#include <cstdio>

static const int LEAD = 9, LOOP = 4, TRAIL = 9, CYCLES = LEAD + LOOP + TRAIL;
static const int LEAD_X[LEAD] = {0,0,0,0,1,0,0,0,0};   // 000010000
static const int LEAD_Y[LEAD] = {0,0,0,0,0,0,0,0,0};   // 000000000

SC_MODULE(Testbench) {
    sc_in<bool>     clk;
    sc_signal<bool> x, x_bar, y, y_bar;
    sc_signal<int>  x_int, y_int;                // одоогийн тестийн утга
    sc_signal<bool> lead, loop, trail;           // фазын дохио
    MustEucl4*      dut;

    // шалгалт: гаралтын битийг цуглуулах
    int cycle = 0, test = 0, cur_x = 0, cur_y = 0;
    unsigned long acc_u = 0, acc_v = 0, acc_z = 0;
    int errors = 0, checked = 0;

    void stimulus() {
        // тест, цикл тодорхойлох
        int n = 0;  // тестийн дугаар -> (x,y)
        for (int xx = 0; xx < 16; ++xx)
            for (int yy = 0; yy <= xx; ++yy, ++n)
                if (n == test) { cur_x = xx; cur_y = yy; }
        int bx = 0, by = 0;
        if (cycle < LEAD)                { bx = LEAD_X[cycle]; by = LEAD_Y[cycle]; }
        else if (cycle < LEAD + LOOP)    { int i = cycle - LEAD; bx = (cur_x >> i) & 1; by = (cur_y >> i) & 1; }
        x.write(bx); x_bar.write(!bx); y.write(by); y_bar.write(!by);
        x_int.write(cur_x); y_int.write(cur_y);
        lead.write(cycle < LEAD); loop.write(cycle >= LEAD && cycle < LEAD + LOOP); trail.write(cycle >= LEAD + LOOP);
    }

    // Гаралтыг шалгах. Нейрон бүр 1 цикл тул: хоцролтын гинж 4, нэмэгч 2 ->
    // u, v-ийн LSB цикл 10-д, z-ийн LSB цикл 12-т гарна (анхны бичлэгтэй ижил).
    static const int T_U = 10, T_Z = 12;
    void check() {
        if (cycle >= T_U && cycle < T_U + 9) { int i = cycle - T_U; acc_u |= (unsigned long)dut->u.read() << i; acc_v |= (unsigned long)dut->v.read() << i; }
        if (cycle >= T_Z && cycle < T_Z + 9) { int i = cycle - T_Z; acc_z |= (unsigned long)dut->z.read() << i; }
        if (cycle == CYCLES - 1) {
            unsigned long eu = 15UL * cur_x, ev = 6UL * cur_y, ez = eu + ev;
            bool ok = (acc_u == eu) && (acc_v == ev) && (acc_z == ez);
            if (!ok) { ++errors; printf("FAIL x=%2d y=%2d  u=%lu (exp %lu)  v=%lu (exp %lu)  z=%lu (exp %lu)\n", cur_x, cur_y, acc_u, eu, acc_v, ev, acc_z, ez); }
            ++checked; acc_u = acc_v = acc_z = 0;
        }
    }

    void tick() {
        // Дараалал: эхлээд өмнөх циклийн гаралтыг шалгах, дараа нь шинэ оролт өгөх
        check();
        if (++cycle == CYCLES) { cycle = 0; ++test; }
        if (test == 136) { printf("checked %d tests, %d errors\n", checked, errors); sc_stop(); return; }
        stimulus();
    }

    SC_CTOR(Testbench)
        : x("x"), x_bar("x_bar"), y("y"), y_bar("y_bar"), x_int("x_int"), y_int("y_int"),
          lead("lead"), loop("loop"), trail("trail")
    {
        dut = new MustEucl4("musteucl4");
        dut->clk(clk); dut->x(x); dut->x_bar(x_bar); dut->y(y); dut->y_bar(y_bar);
        SC_METHOD(tick); sensitive << clk.neg();     // оролтыг сөрөг фронтод өөрчилнө
        dont_initialize();
        stimulus();                                   // t=0-ийн оролт
    }
};

int sc_main(int, char**) {
    sc_clock clk("clock", 10, SC_NS);   // 1 цикл = 10 ns (анхны бичлэгтэй ижил)
    Testbench tb("tb"); tb.clk(clk);

    sc_trace_file* tf = sc_create_vcd_trace_file("musteucl4");
    tf->set_time_unit(1, SC_PS);
    sc_trace(tf, clk, "clock");
    sc_trace(tf, tb.x, "x"); sc_trace(tf, tb.x_bar, "x_bar");
    sc_trace(tf, tb.dut->x1, "x1"); sc_trace(tf, tb.dut->x1_bar, "x1_bar");
    sc_trace(tf, tb.dut->x2, "x2"); sc_trace(tf, tb.dut->x2_bar, "x2_bar");
    sc_trace(tf, tb.dut->x3, "x3"); sc_trace(tf, tb.dut->x3_bar, "x3_bar");
    sc_trace(tf, tb.dut->x4, "x4"); sc_trace(tf, tb.dut->x4_bar, "x4_bar");
    sc_trace(tf, tb.y, "y"); sc_trace(tf, tb.y_bar, "y_bar");
    sc_trace(tf, tb.dut->y1, "y1"); sc_trace(tf, tb.dut->y1_bar, "y1_bar");
    sc_trace(tf, tb.dut->y2, "y2"); sc_trace(tf, tb.dut->y2_bar, "y2_bar");
    sc_trace(tf, tb.dut->u, "u"); sc_trace(tf, tb.dut->u_bar, "u_bar");
    sc_trace(tf, tb.dut->v, "v"); sc_trace(tf, tb.dut->v_bar, "v_bar");
    sc_trace(tf, tb.dut->z, "z"); sc_trace(tf, tb.dut->z_bar, "z_bar");
    sc_trace(tf, tb.x_int, "x_int"); sc_trace(tf, tb.y_int, "y_int");
    sc_trace(tf, tb.lead, "lead"); sc_trace(tf, tb.loop, "loop"); sc_trace(tf, tb.trail, "trail");
    sc_trace(tf, tb.dut->addx->c, "addx.c"); sc_trace(tf, tb.dut->addx->c_bar, "addx.c_bar");
    sc_trace(tf, tb.dut->addy->c, "addy.c"); sc_trace(tf, tb.dut->addy->c_bar, "addy.c_bar");
    sc_trace(tf, tb.dut->addz->c, "addz.c"); sc_trace(tf, tb.dut->addz->c_bar, "addz.c_bar");

    sc_start();
    sc_close_vcd_trace_file(tf);
    return 0;
}
