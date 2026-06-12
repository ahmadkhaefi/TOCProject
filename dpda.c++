#include "dpda.h++"
#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <map>
#include <set>
#include <algorithm>
#include <format>
#include <optional>

std::optional<std::vector<State>::iterator> DPDA::find_state(const State& state) {
    auto it = std::find(states.begin(), states.end(), state);

    if (it == states.end()) return std::nullopt;

    return it;
}
std::optional<std::vector<char>::iterator> DPDA::find_input_symbol(char symbol) {
    auto it = std::find(input_alphabet.begin(), input_alphabet.end(), symbol);

    if (it == input_alphabet.end()) return std::nullopt;

    return it;
}
std::optional<std::vector<char>::iterator> DPDA::find_stack_symbol(char symbol) {
    auto it = std::find(stack_alphabet.begin(), stack_alphabet.end(), symbol);

    if (it == stack_alphabet.end()) return std::nullopt;

    return it;
}

std::string DPDA::push_string_to_label(const std::string& push) const {
    
    if (push.empty()) return "eps";
    return push;
}

void DPDA::add_state(const State& state) {
    if (is_built) throw DPDAError::dpda_already_built();
    if (find_state(state).has_value()) throw DPDAError::state_already_exists(state);

    states.push_back(state);
}
void DPDA::add_input_symbol(char symbol) {
    if (is_built) throw DPDAError::dpda_already_built();
    if (find_input_symbol(symbol).has_value()) throw DPDAError::symbol_already_exists(symbol);

    input_alphabet.push_back(symbol);
}
void DPDA::add_stack_symbol(char symbol) {
    if (is_built) throw DPDAError::dpda_already_built();
    if (find_stack_symbol(symbol).has_value()) throw DPDAError::stack_symbol_already_exists(symbol);

    stack_alphabet.push_back(symbol);
}
void DPDA::set_start_state(const State& start) {
    if (is_built) throw DPDAError::dpda_already_built();
    if (start_state) throw DPDAError::start_state_already_exists();
    if (!find_state(start).has_value()) throw DPDAError::state_not_found(start);

    start_state = start;
}
void DPDA::set_initial_stack_symbol(char symbol) {
    if (is_built) throw DPDAError::dpda_already_built();

    initial_stack_symbol = symbol;
}
void DPDA::add_final_state(const State& final) {
    if (is_built) throw DPDAError::dpda_already_built();
    if (!find_state(final).has_value()) throw DPDAError::state_not_found(final);

    final_states.push_back(final);
}
void DPDA::set_acceptance_mode(const std::string& mode) {
    if (is_built) throw DPDAError::dpda_already_built();

    if (mode == "final") {
        acceptance_mode = AcceptanceMode::FINAL_STATE;
    } else if (mode == "empty") {
        acceptance_mode = AcceptanceMode::EMPTY_STACK;
    } else {
        throw DPDAError::invalid_acceptance_mode(mode);
    }
}
void DPDA::add_transition(const State& origin, char input, char stack_top, const State& destination, const std::string& push) {
    if (is_built) throw DPDAError::dpda_already_built();

    if (input != LAMBDA && !find_input_symbol(input).has_value())
        throw DPDAError::symbol_not_found(input);

    if (!find_stack_symbol(stack_top).has_value())
        throw DPDAError::stack_symbol_not_found(stack_top);

    for (char c : push) {
        if (!find_stack_symbol(c).has_value())
            throw DPDAError::stack_symbol_not_found(c);
    }

    if (!find_state(origin).has_value()) throw DPDAError::state_not_found(origin);
    if (!find_state(destination).has_value()) throw DPDAError::state_not_found(destination);

    PDATransitionKey key{origin, input, stack_top};

    if (transitions.find(key) != transitions.end())
        throw DPDAError::transition_already_exists(origin, input, stack_top);

    transitions[key] = {destination, push};
}

void DPDA::build() {
    if (name.empty()) throw DPDAError::dpda_name_not_set();
    if (states.empty()) throw DPDAError::dpda_no_states();
    if (!start_state) throw DPDAError::dpda_no_start_state();
    if (input_alphabet.empty()) throw DPDAError::dpda_no_input_alphabet();
    if (stack_alphabet.empty()) throw DPDAError::dpda_no_stack_alphabet();
    if (initial_stack_symbol == LAMBDA) throw DPDAError::initial_stack_symbol_not_set();
    if (!find_stack_symbol(initial_stack_symbol).has_value())
        throw DPDAError::initial_stack_symbol_not_in_alphabet();

    if (acceptance_mode == AcceptanceMode::FINAL_STATE && final_states.empty()) {
        throw DPDAError::dpda_no_final_states();
    }

    std::map<std::pair<State, char>, bool> has_lambda;
    std::map<std::pair<State, char>, bool> has_symbol;

    for (const auto& [key, value] : transitions) {
        std::pair<State, char> sig{key.state, key.stack_top};

        if (key.input == LAMBDA) {
            has_lambda[sig] = true;
        } else {
            has_symbol[sig] = true;
        }
    }

    for (const auto& [sig, _] : has_lambda) {
        if (has_symbol.count(sig)) {
            throw DPDAError::nondeterministic_transition(sig.first, sig.second);
        }
    }

    is_built = true;
}

DPDA::DPDA(std::string n) : name(std::move(n)) {}

void DPDA::simulate(const std::string& input) {
    if (!is_built) throw DPDAError::dpda_not_built();

    std::cout << std::format("Input string: {}.\n", input);
    std::cout << std::format("Acceptance mode: {}.\n", acceptance_mode == AcceptanceMode::FINAL_STATE ? "final" : "empty");

    State current = start_state;
    std::vector<char> stack{initial_stack_symbol};

    std::cout << std::format("State: {} , Stack: {}.\n", current.name, stack_to_string(stack));

    size_t pos = 0;
    bool rejected = false;
    std::string reject_reason;

    for (char c : input) {
        if (!find_input_symbol(c).has_value()) {
            std::cout << std::format("Symbol '{}' is not in the input alphabet.\n", c);
            std::cout << "Result: Rejected.\n";
            return;
        }
    }

    while (true) {
        char stack_top = stack.empty() ? LAMBDA : stack.back();

        PDATransitionKey lambda_key{current, LAMBDA, stack_top};
        auto lambda_it = transitions.find(lambda_key);

        if (lambda_it != transitions.end() && stack_top != LAMBDA) {
            const auto& [next_state, push] = lambda_it->second;

            if (!stack.empty()) stack.pop_back();
            for (auto it2 = push.rbegin(); it2 != push.rend(); ++it2) stack.push_back(*it2);

            std::cout << std::format("Read lambda -> {} {}\n",
                push.empty() ? "pop" : "push", push_string_to_label(push));
            std::cout << std::format("State: {} , Stack: {}\n", next_state.name, stack_to_string(stack));

            current = next_state;
            continue;
        }

        if (pos < input.size()) {
            char symbol = input[pos];

            PDATransitionKey key{current, symbol, stack_top};
            auto it = transitions.find(key);

            if (it == transitions.end() || stack_top == LAMBDA) {
                rejected = true;
                reject_reason = std::format(
                    "no transition defined for (state {}, input '{}', stack top '{}')",
                    current.name, symbol, stack_top == LAMBDA ? '-' : stack_top
                );
                break;
            }

            const auto& [next_state, push] = it->second;

            if (!stack.empty()) stack.pop_back();
            for (auto it2 = push.rbegin(); it2 != push.rend(); ++it2) stack.push_back(*it2);

            std::cout << std::format("Read {} -> {} {}.\n", symbol, push.empty() ? "pop" : "push", push_string_to_label(push));
            std::cout << std::format("State: {} , Stack: {}.\n", next_state.name, stack_to_string(stack));

            current = next_state;
            pos++;
            continue;
        }

        break;
    }

    std::cout << std::format("Halted at state: {}.\n", current.name);

    if (rejected) {
        std::cout << std::format("Execution halted early: {}.\n", reject_reason);
        std::cout << "Result: Rejected.\n";
        return;
    }

    if (pos < input.size()) {
        std::cout << "Result: Rejected.\n";
        return;
    }

    bool accepted = false;

    if (acceptance_mode == AcceptanceMode::FINAL_STATE) {
        accepted = std::find(final_states.begin(), final_states.end(), current) != final_states.end();
    } else { 
        accepted = stack.empty();
    }

    std::cout << "Result: " << (accepted ? "Accepted" : "Rejected") << ".\n";
}
