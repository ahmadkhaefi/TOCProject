#pragma once

#include <istream>
#include "dfa.h++"
#include "dpda.h++"

void configure_dfa(std::istream&, DFA&);
void simulate_dfa_strings(std::istream&, DFA&);

void configure_dpda(std::istream&, DPDA&);
void simulate_dpda_strings(std::istream&, DPDA&);
