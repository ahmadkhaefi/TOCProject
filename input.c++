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

void simulate_dfa_strings(std::istream& in, DFA& dfa) {
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

void configure_dpda(std::istream& in, DPDA& dpda) {
    std::string line;

    // std::cout << "States: ";
    if (std::getline(std::cin, line)) {
        std::stringstream ss(line);
        std::string state;
        while (ss >> state) dpda.add_state(State(state));
    }
    
    // std::cout << "Input alphabet: ";
    if (std::getline(std::cin, line)) {
        std::stringstream ss(line);
        std::string symbol;
        while (ss >> symbol) dpda.add_input_symbol(symbol[0]);
    }

    // std::cout << "Stack alphabet: ";
    if (std::getline(std::cin, line)) {
        std::stringstream ss(line);
        std::string symbol;
        while (ss >> symbol) dpda.add_stack_symbol(symbol[0]);
    }

    // std::cout << "Start state: ";
    if (std::getline(std::cin, line)) {
        std::stringstream ss(line);
        std::string start;
        ss >> start;
        dpda.set_start_state(State(start));
    }
    
    // std::cout << "Initial stack symbol: ";
    if (std::getline(std::cin, line)) {
        std::stringstream ss(line);
        std::string symbol;
        ss >> symbol;
        dpda.set_initial_stack_symbol(symbol[0]);
    }

    // std::cout << "Final states: ";
    if (std::getline(std::cin, line)) {
        std::stringstream ss(line);
        std::string state;
        while (ss >> state) dpda.add_final_state(State(state));
    }

    // std::cout << "Acceptance mode: ";
    if (std::getline(std::cin, line)) {
        std::stringstream ss(line);
        std::string mode;
        ss >> mode;
        dpda.set_acceptance_mode(mode);
    }
   
    // std::cout << "Number of transitions: ";
    int number_of_transitions = 0;
    if (std::getline(std::cin, line)) {
        std::stringstream ss(line);
        ss >> number_of_transitions;
    }

    for (int i = 0; i < number_of_transitions; i++) {
        if (std::getline(std::cin, line)) {
            std::stringstream ss(line);
            std::string origin, input_sym, stack_top, destination, push;

            ss >> origin >> input_sym >> stack_top >> destination >> push;

            char input_char = (input_sym == "eps" || input_sym == "lambda") ? LAMBDA : input_sym[0];
            std::string push_str = (push == "eps" || push == "lambda") ? "" : push;

            dpda.add_transition(State(origin), input_char, stack_top[0], State(destination), push_str);
        }
    }

    dpda.build();
}

void simulate_dpda_strings(std::istream& in, DPDA& dpda) {
    std::string line;

    int number_of_strings = 0;

    if (std::getline(std::cin, line)) {
        std::stringstream ss(line);
        ss >> number_of_strings;
    }

    for (int i = 0; i < number_of_strings; i++) {
        if (std::getline(std::cin, line)) {
            std::stringstream ss(line);
            std::string str;
            ss >> str;

            dpda.simulate(str);
        }
    }
}
