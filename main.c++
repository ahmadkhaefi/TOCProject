#include <iostream>
#include "dfa.h++"
#include "dpda.h++"
#include "input.h++"

int main() {
    // DFA dfa("M");
    // configure_dfa(std::cin, dfa);
    // simulate_dfa_strings(std::cin, dfa);

    DPDA dpda("M");
    configure_dpda(std::cin, dpda);
    simulate_dpda_strings(std::cin, dpda);
}
