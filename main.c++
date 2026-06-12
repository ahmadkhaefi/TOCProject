#include <iostream>
#include "dfa.h++"
#include "input.h++"

int main() {
    DFA dfa("M");
    configure_dfa(std::cin, dfa);
    simulate_strings(std::cin, dfa);
}
