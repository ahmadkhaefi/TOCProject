#pragma once
#include <istream>
#include "dfa.h++"

void configure_dfa(std::istream&, DFA&);
void simulate_strings(std::istream&, DFA&);
