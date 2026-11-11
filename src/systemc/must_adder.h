// must_adder.h — MUST-Adder: давхар шугамт логикт HP/LP нейроноор угсарсан
// цуваа бүтэн нэмэгч (Ochirbat, Ishdorj, Cichon, J. Membrane Computing 2020).
//
//   U  = LP(A, B)            (XOR)            U̅  = LP(A̅, B̅)
//   A' = LP(A),  B' = LP(B)  (хоцролт)        A̅', B̅' мөн адил
//   S  = LP(U, C)            (нийлбэр)        S̅  = LP(U̅, C̅)
//   C  = HP(A', B', C)       (оруулга, буцах холбоостой)   C̅ = HP(A̅', B̅', C̅)
//
// Нийт 8 LP + 2 HP = 10 нейрон, гүн 2 давхарга -> нийлбэрийн латенти 2 цикл.
// Оруулга C нь дотоод дохио: гаднаас удирдагдахгүй, зөвхөн (A,B)=(0,0) -> 0,
// (1,1) -> 1 оролтоор тохируулагдана.
#ifndef MUST_ADDER_H
#define MUST_ADDER_H
#include "neuron.h"

SC_MODULE(MustAdder) {
    sc_in<bool>  clk;
    sc_in<bool>  a, a_bar, b, b_bar;     // давхар шугамт оролт
    sc_out<bool> s, s_bar;               // давхар шугамт гаралт
    sc_signal<bool> c, c_bar;            // дотоод оруулга (VCD-д харагдана)

    // дотоод дохио
    sc_signal<bool> u, u_bar, ap, ap_bar, bp, bp_bar;

    Neuron *nU, *nUb, *nAp, *nApb, *nBp, *nBpb, *nS, *nSb, *nC, *nCb;

    SC_CTOR(MustAdder)
        : c("c"), c_bar("c_bar"), u("u"), u_bar("u_bar"),
          ap("ap"), ap_bar("ap_bar"), bp("bp"), bp_bar("bp_bar")
    {
        // --- 1-р давхарга
        nU   = make_lp("U",     2); nU->clk(clk);   nU->dendrite[0](a);     nU->dendrite[1](b);     nU->synapse(u);
        nUb  = make_lp("U_bar", 2); nUb->clk(clk);  nUb->dendrite[0](a_bar);nUb->dendrite[1](b_bar);nUb->synapse(u_bar);
        nAp  = make_lp("Ap",    1); nAp->clk(clk);  nAp->dendrite[0](a);    nAp->synapse(ap);
        nApb = make_lp("Ap_bar",1); nApb->clk(clk); nApb->dendrite[0](a_bar);nApb->synapse(ap_bar);
        nBp  = make_lp("Bp",    1); nBp->clk(clk);  nBp->dendrite[0](b);    nBp->synapse(bp);
        nBpb = make_lp("Bp_bar",1); nBpb->clk(clk); nBpb->dendrite[0](b_bar);nBpb->synapse(bp_bar);
        // --- 2-р давхарга
        nS   = make_lp("S",     2); nS->clk(clk);   nS->dendrite[0](u);     nS->dendrite[1](c);     nS->synapse(s);
        nSb  = make_lp("S_bar", 2); nSb->clk(clk);  nSb->dendrite[0](u_bar);nSb->dendrite[1](c_bar);nSb->synapse(s_bar);
        nC   = make_hp("C",     3); nC->clk(clk);   nC->dendrite[0](ap);    nC->dendrite[1](bp);    nC->dendrite[2](c);     nC->synapse(c);
        nCb  = make_hp("C_bar", 3); nCb->clk(clk);  nCb->dendrite[0](ap_bar);nCb->dendrite[1](bp_bar);nCb->dendrite[2](c_bar);nCb->synapse(c_bar);
    }
};

#endif
