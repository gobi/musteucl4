// neuron.h — HP/LP үндсэн нейрон (Xu et al. 2014; Ochirbat et al. 2020)
//
// SN P системийн семантик: алхам (цикл) бүрт нейрон дендритээрээ ирсэн
// спайкийг тоолж, дүрмээ хэрэгжүүлээд гаралтын спайкаа ДАРААГИЙН алхамд
// хүргэнэ. Тиймээс нейрон бүр нэг циклийн хоцролттой регистр мэт ажиллана.
//
//   HP (High-Pass):  спайк >= 2  -> a      (a^k -> a, k>=2;  a -> λ)
//   LP (Low-Pass):   спайк == 1  -> a      (a -> a;  a^k -> λ, k>=2)
//
// Нэг оролттой LP = хоцролт (z^-1), хоёр оролттой LP = XOR,
// хоёр оролттой HP = AND, гурван оролттой HP = олонхи (majority).
#ifndef NEURON_H
#define NEURON_H
#include <systemc.h>

SC_MODULE(Neuron) {
    sc_in<bool>              clk;
    sc_vector< sc_in<bool> > dendrite;   // оролтууд
    sc_out<bool>             synapse;    // ганц гаралт

    const bool is_hp;                    // true: HP, false: LP

    void step() {
        int spikes = 0;
        for (unsigned i = 0; i < dendrite.size(); ++i)
            if (dendrite[i].read()) ++spikes;
        synapse.write(is_hp ? (spikes >= 2) : (spikes == 1));
    }

    SC_HAS_PROCESS(Neuron);
    Neuron(sc_module_name name, bool hp, unsigned n_inputs)
        : sc_module(name), dendrite("dendrite", n_inputs), is_hp(hp)
    {
        SC_METHOD(step);
        sensitive << clk.pos();          // алхам бүрт нэг удаа
        dont_initialize();
    }
};

// Товч үүсгэгчид
inline Neuron* make_hp(const char* name, unsigned n) { return new Neuron(name, true,  n); }
inline Neuron* make_lp(const char* name, unsigned n) { return new Neuron(name, false, n); }

#endif
