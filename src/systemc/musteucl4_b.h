// musteucl4_b.h — Хувилбар B: хасагчгүй MUST-Euclidean
//   w' = 8x + 3y = (x<<3) + (y<<1) + y,   w ~ w'/8   (a = 1, b = 3/8, алдаа <= 6.80%)
//
//   x --[z^-1]x1-- ... --[z^-1]x5 --> ADDz'.A      (x<<3, дээр нь ADDy'-ийн 2 алхмын латентийг нөхөх 2 хоцролт)
//   y --[z^-1]y1 --> ADDy'.A,   y --> ADDy'.B       (2y + y = 3y)
//   ADDz' = x5 + ADDy'  -> z = w'
//
// Хасагч, урвуулагч байхгүй; урьдал дараалал зөвхөн "00" (хоёр нэмэгчийн оруулгыг
// дараалан цэвэрлэх). Идеал (латентигүй) загварт x-д 3 хоцролт хангалттай; бодит
// нейроны загварт MUST-Adder-ийн нийлбэр хүлээн авснаас дараагийн нэмэгчид хүрэх
// хүртэл 2 алхам зарцуулдаг тул ADDy'-г дайраагүй x замд 2 хоцролт нэмж 5 болгов.
// (Хувилбар A-д ADDx, ADDy ижил латентитай тул ийм нөхөлт шаардагдахгүй.)
// Нийт нейрон: хоцролт 12 + 2 x 10 = 32.
#ifndef MUSTEUCL4_B_H
#define MUSTEUCL4_B_H
#include "must_adder.h"

SC_MODULE(MustEucl4B) {
    sc_in<bool>  clk;
    sc_in<bool>  x, x_bar, y, y_bar;
    sc_signal<bool> v, v_bar;      // ADDy' гаралт = 3y
    sc_signal<bool> z, z_bar;      // ADDz' гаралт = w' = 8x + 3y
    sc_signal<bool> x1, x1_bar, x2, x2_bar, x3, x3_bar, x4, x4_bar, x5, x5_bar, y1, y1_bar;

    MustAdder *addy, *addz;
    Neuron *dx[5], *dxb[5], *dy, *dyb;

    SC_CTOR(MustEucl4B)
        : v("v"), v_bar("v_bar"), z("z"), z_bar("z_bar"),
          x1("x1"), x1_bar("x1_bar"), x2("x2"), x2_bar("x2_bar"), x3("x3"), x3_bar("x3_bar"), x4("x4"), x4_bar("x4_bar"), x5("x5"), x5_bar("x5_bar"),
          y1("y1"), y1_bar("y1_bar")
    {
        sc_signal<bool>* xs [6] = { nullptr, &x1, &x2, &x3, &x4, &x5 };
        sc_signal<bool>* xsb[6] = { nullptr, &x1_bar, &x2_bar, &x3_bar, &x4_bar, &x5_bar };
        for (int k = 1; k <= 5; ++k) {
            char n1[16], n2[16]; sprintf(n1, "dx%d", k); sprintf(n2, "dx%d_bar", k);
            dx[k-1]  = make_lp(n1, 1); dx[k-1]->clk(clk);  dx[k-1]->synapse(*xs[k]);
            dxb[k-1] = make_lp(n2, 1); dxb[k-1]->clk(clk); dxb[k-1]->synapse(*xsb[k]);
            if (k == 1) { dx[0]->dendrite[0](x);  dxb[0]->dendrite[0](x_bar); }
            else        { dx[k-1]->dendrite[0](*xs[k-1]); dxb[k-1]->dendrite[0](*xsb[k-1]); }
        }
        dy  = make_lp("dy1", 1);     dy->clk(clk);  dy->dendrite[0](y);      dy->synapse(y1);
        dyb = make_lp("dy1_bar", 1); dyb->clk(clk); dyb->dendrite[0](y_bar); dyb->synapse(y1_bar);

        addy = new MustAdder("addy"); addy->clk(clk);
        addy->a(y1); addy->a_bar(y1_bar);          // 2y
        addy->b(y);  addy->b_bar(y_bar);           //  y
        addy->s(v);  addy->s_bar(v_bar);

        addz = new MustAdder("addz"); addz->clk(clk);
        addz->a(x5); addz->a_bar(x5_bar);          // 8x (x<<3 + 2 алхмын латентийн нөхөлт)
        addz->b(v);  addz->b_bar(v_bar);           // 3y
        addz->s(z);  addz->s_bar(z_bar);
    }
};

#endif
