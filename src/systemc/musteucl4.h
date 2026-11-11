// musteucl4.h — MUST-Euclidean (4 бит): w = 15x + 6y = (x<<4) - x + (y<<1) + (y<<2)
//
//   x --[z^-1]x1--[z^-1]x2--[z^-1]x3--[z^-1]x4 --> ADDx.A      (x<<4)
//   x --(шугам солих)--> x̅ ------------------> ADDx.B      (-x = x̅ + 1)
//   y --[z^-1]y1 --> ADDy.A, y1--[z^-1]y2 --> ADDy.B       (2y + 4y)
//   ADDz = ADDx + ADDy  -> z = w
//
// Хоцролт бүр нэг оролттой LP нейрон (давхар шугамд хоёр). Нийт нейрон:
// хоцролт 12 + 3 x 10 (MUST-Adder) = 42.
#ifndef MUSTEUCL4_H
#define MUSTEUCL4_H
#include "must_adder.h"

SC_MODULE(MustEucl4) {
    sc_in<bool>  clk;
    sc_in<bool>  x, x_bar, y, y_bar;
    // гаралтууд (дотоод дохио; тестбенч шууд уншиж, VCD-д бичнэ)
    sc_signal<bool> u, u_bar;   // ADDx гаралт = 15x
    sc_signal<bool> v, v_bar;   // ADDy гаралт = 6y
    sc_signal<bool> z, z_bar;   // ADDz гаралт = w

    // хоцролтын зангилаанууд (VCD-д харагдана)
    sc_signal<bool> x1, x1_bar, x2, x2_bar, x3, x3_bar, x4, x4_bar;
    sc_signal<bool> y1, y1_bar, y2, y2_bar;

    MustAdder *addx, *addy, *addz;
    Neuron *dx[4], *dxb[4], *dy[2], *dyb[2];

    SC_CTOR(MustEucl4)
        : u("u"), u_bar("u_bar"), v("v"), v_bar("v_bar"), z("z"), z_bar("z_bar"),
          x1("x1"), x1_bar("x1_bar"), x2("x2"), x2_bar("x2_bar"),
          x3("x3"), x3_bar("x3_bar"), x4("x4"), x4_bar("x4_bar"),
          y1("y1"), y1_bar("y1_bar"), y2("y2"), y2_bar("y2_bar")
    {
        // --- x-ийн хоцролтын гинж (x -> x1 -> x2 -> x3 -> x4)
        sc_signal<bool>* xs [5] = { nullptr, &x1, &x2, &x3, &x4 };
        sc_signal<bool>* xsb[5] = { nullptr, &x1_bar, &x2_bar, &x3_bar, &x4_bar };
        for (int k = 1; k <= 4; ++k) {
            char n1[16], n2[16]; sprintf(n1, "dx%d", k); sprintf(n2, "dx%d_bar", k);
            dx[k-1]  = make_lp(n1, 1); dx[k-1]->clk(clk);  dx[k-1]->synapse(*xs[k]);
            dxb[k-1] = make_lp(n2, 1); dxb[k-1]->clk(clk); dxb[k-1]->synapse(*xsb[k]);
            if (k == 1) { dx[0]->dendrite[0](x);  dxb[0]->dendrite[0](x_bar); }
            else        { dx[k-1]->dendrite[0](*xs[k-1]); dxb[k-1]->dendrite[0](*xsb[k-1]); }
        }
        // --- y-ийн хоцролтын гинж (y -> y1 -> y2)
        dy[0]  = make_lp("dy1", 1);     dy[0]->clk(clk);  dy[0]->dendrite[0](y);      dy[0]->synapse(y1);
        dyb[0] = make_lp("dy1_bar", 1); dyb[0]->clk(clk); dyb[0]->dendrite[0](y_bar); dyb[0]->synapse(y1_bar);
        dy[1]  = make_lp("dy2", 1);     dy[1]->clk(clk);  dy[1]->dendrite[0](y1);     dy[1]->synapse(y2);
        dyb[1] = make_lp("dy2_bar", 1); dyb[1]->clk(clk); dyb[1]->dendrite[0](y1_bar);dyb[1]->synapse(y2_bar);

        // --- ADDx: A = x4 (x<<4),  B = x̅ (шугам солих = үгүйсгэл, нейронгүй)
        addx = new MustAdder("addx"); addx->clk(clk);
        addx->a(x4); addx->a_bar(x4_bar);
        addx->b(x_bar); addx->b_bar(x);            // <-- урвуулагч: шугам солисон
        addx->s(u); addx->s_bar(u_bar);

        // --- ADDy: A = y1 (y<<1), B = y2 (y<<2)
        addy = new MustAdder("addy"); addy->clk(clk);
        addy->a(y1); addy->a_bar(y1_bar);
        addy->b(y2); addy->b_bar(y2_bar);
        addy->s(v); addy->s_bar(v_bar);

        // --- ADDz: A = u, B = v
        addz = new MustAdder("addz"); addz->clk(clk);
        addz->a(u); addz->a_bar(u_bar);
        addz->b(v); addz->b_bar(v_bar);
        addz->s(z); addz->s_bar(z_bar);
    }
};

#endif
