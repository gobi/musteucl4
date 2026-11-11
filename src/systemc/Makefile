# Makefile — MUST-Euclidean SystemC загвар
#
# Linux (Debian/Ubuntu):  sudo apt install libsystemc-dev  ->  make
# macOS (Homebrew):       brew install systemc              ->  make SYSTEMC=$(brew --prefix systemc)
# Гараар суулгасан бол:   make SYSTEMC=/usr/local/systemc-2.3.4
#
# Ажиллуулах:  ./musteucl4        (musteucl4.vcd үүснэ)
# Харах:       gtkwave musteucl4.vcd
# Хувилбар B:  ./musteucl4_b      (musteucl4_b.vcd)

CXX      ?= g++
CXXFLAGS ?= -std=c++17 -O2 -Wall -Wno-unused-variable
SYSTEMC  ?=

ifeq ($(SYSTEMC),)
  INC  :=
  LIBS := -lsystemc
else
  INC  := -I$(SYSTEMC)/include
  LIBDIR := $(firstword $(wildcard $(SYSTEMC)/lib-* $(SYSTEMC)/lib64 $(SYSTEMC)/lib))
  LIBS := -L$(LIBDIR) -Wl,-rpath,$(LIBDIR) -lsystemc
endif

all: musteucl4 musteucl4_b

musteucl4: testbench.cpp neuron.h must_adder.h musteucl4.h
	$(CXX) $(CXXFLAGS) $(INC) testbench.cpp -o $@ $(LIBS)

musteucl4_b: testbench_b.cpp neuron.h must_adder.h musteucl4_b.h
	$(CXX) $(CXXFLAGS) $(INC) testbench_b.cpp -o $@ $(LIBS)

run: musteucl4
	./musteucl4

clean:
	rm -f musteucl4 musteucl4_b *.vcd *.o

.PHONY: all run clean
