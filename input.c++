#include "input.h++"
#include <iostream>
#include <sstream>

void configure_dfa(std::istream& in, DFA& dfa) {
    std::string line;

    // STATES
    std::cout << "STATES: ";

    if (std::getline(in, line)) {
        std::stringstream ss(line);
        std::string state;

        while (ss >> state) dfa.add_state(State(state));
    }

    // ALPHABET
    std::cout << "ALPHABET: ";

    if (std::getline(in, line)) {
        std::stringstream ss(line);
        std::string symbol;

        while (ss >> symbol) dfa.add_symbol(symbol[0]);
    }

    // START STATE
    std::cout << "START STATE: ";

    if (std::getline(in, line)) {
        std::stringstream ss(line);
        std::string start;

        ss >> start;

        dfa.set_start_state(State(start));
    }

    // FINAL STATES
    std::cout << "FINAL STATES: ";

    if (std::getline(in, line)) {
        std::stringstream ss(line);
        std::string state;

        while (ss >> state) dfa.add_final_state(State(state));
    }

    // TRANSITIONS
    std::cout << "NUMBER OF TRANSITIONS: ";

    int number_of_transitions;

    if (std::getline(in, line)) {
        std::stringstream ss(line);

        ss >> number_of_transitions;
    }

    // int maximum_number_of_transitions = dfa.size() * alphabet.size();

    // if (number_of_transitions > maximum_number_of_transitions) {
    //     throw std::runtime_error(std::format("Number of transitions must not exceed {}.", maximum_number_of_transitions));
    // }

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
    std::cout << "NUMBER OF STRINGS: ";

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
