#include "input.h++"
#include <iostream>
#include <sstream>

void configure_dfa(std::istream& in, DFA& dfa) {
    std::string line;

    // STATES
    if (std::getline(in, line)) {
        std::stringstream ss(line);
        std::string state;

        while (ss >> state) dfa.add_state(State(state));
    }

    // ALPHABET
    if (std::getline(in, line)) {
        std::stringstream ss(line);
        std::string symbol;

        while (ss >> symbol) dfa.add_symbol(symbol[0]);
    }

    // START STATE
    if (std::getline(in, line)) {
        std::stringstream ss(line);
        std::string start;

        ss >> start;

        dfa.set_start_state(State(start));
    }

    // FINAL STATES
    if (std::getline(in, line)) {
        std::stringstream ss(line);
        std::string state;

        while (ss >> state) dfa.add_final_state(State(state));
    }

    // TRANSITIONS
    int number_of_transitions;

    if (std::getline(in, line)) {
        std::stringstream ss(line);

        ss >> number_of_transitions;
    }

    for (size_t i = 0; i < number_of_transitions; i++) {
        if (std::getline(in, line)) {
            std::stringstream ss(line);
            std::string origin, symbol, destination;

            ss >> origin >> symbol >> destination;
            dfa.add_transition(State(origin), symbol[0], State(destination));
        }
    }

    dfa.build();
}

void simulate_strings(std::istream& in, DFA& dfa) {
    std::string line;

    // STRINGS
    int number_of_strings;

    if (std::getline(in, line)) {
        std::stringstream ss(line);

        ss >> number_of_strings;
    }

    for (size_t i = 0; i < number_of_strings; i++) {
        if (std::getline(in, line)) {
            std::stringstream ss(line);
            std::string str;

            ss >> str;

            dfa.simulate(str);
        }
    }
}
