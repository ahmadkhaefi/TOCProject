#pragma once

#include "state.h++"
#include "error.h++"
#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <map>
#include <set>
#include <algorithm>
#include <format>
#include <optional>

// constexpr char LAMBDA = '\0';

enum class AcceptanceMode {
    FINAL_STATE,
    EMPTY_STACK
};

struct PDATransitionKey {
    State state;
    char input; 
    char stack_top;

    bool operator<(const PDATransitionKey& other) const {
        if (state != other.state) return state < other.state;
        if (input != other.input) return input < other.input;
        return stack_top < other.stack_top;
    }
};

struct PDATransitionValue {
    State next_state;
    std::string push;
};

struct DPDA {
private:
    std::string name;

    bool is_built = false;

    std::vector<State> states;
    std::vector<char> input_alphabet;
    std::vector<char> stack_alphabet;

    State start_state;
    char initial_stack_symbol = LAMBDA;
    std::vector<State> final_states;

    AcceptanceMode acceptance_mode = AcceptanceMode::FINAL_STATE;

    std::map<PDATransitionKey, PDATransitionValue> transitions;

    std::optional<std::vector<State>::iterator> find_state(const State&);
    std::optional<std::vector<char>::iterator> find_input_symbol(char);
    std::optional<std::vector<char>::iterator> find_stack_symbol(char);
    std::string push_string_to_label(const std::string&) const;
    
    static std::string stack_to_string(const std::vector<char>& stack) {
        std::string out;
        for (auto it = stack.rbegin(); it != stack.rend(); ++it) out += *it;
        return out;
    }

public:
    explicit DPDA(std::string n);

    void add_state(const State&);
    void add_input_symbol(char);
    void add_stack_symbol(char);
    void set_start_state(const State&);
    void set_initial_stack_symbol(char);
    void add_final_state(const State&);
    void set_acceptance_mode(const std::string&);
    void add_transition(const State&, char, char, const State&, const std::string&);
    void build();
    void simulate(const std::string&);
};
