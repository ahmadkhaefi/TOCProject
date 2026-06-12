#include "dfa.h++"
#include <algorithm>
#include <iostream>
#include <format>
#include <queue>

std::optional<std::vector<State>::iterator> DFA::find_state(const State& state) {
    auto it = std::find(states.begin(), states.end(), state);
    if (it == states.end()) return std::nullopt;
    return it;
}

std::optional<std::vector<char>::iterator> DFA::find_symbol(char symbol) {
    auto it = std::find(alphabet.begin(), alphabet.end(), symbol);
    if (it == alphabet.end()) return std::nullopt;
    return it;
}

std::set<State> DFA::find_reachable_states() {
    std::queue<State> queue;
    std::set<State> reachable;

    reachable.insert(start_state);
    queue.push(start_state);

    while (!queue.empty()) {
        auto current = std::move(queue.front());
        queue.pop();

        for (char symbol : alphabet) {
            auto it = transitions.find({current, symbol});
            if (it != transitions.end()) {
                if (reachable.insert(it->second).second) queue.push(it->second);
            }
        }
    }

    return reachable;
}

std::set<State> DFA::find_co_reachable_states() {
    std::map<State, std::vector<State>> reverse;
    for (const auto& [key, dest] : transitions)
        reverse[dest].emplace_back(key.first);

    std::queue<State> queue;
    std::set<State> co_reachable;

    for (const auto& final : final_states) {
        co_reachable.insert(final);
        queue.push(final);
    }

    while (!queue.empty()) {
        auto current = std::move(queue.front());
        queue.pop();

        for (const auto& origin : reverse[current]) {
            if (co_reachable.insert(origin).second) queue.push(origin);
        }
    }

    return co_reachable;
}

std::ostream& operator<<(std::ostream& os, const DFA& dfa) {
    if (!dfa.is_built) throw DFAError::dfa_not_built();

    os << std::format("{} = (Q, Σ, δ, {}, F)\n", dfa.name, dfa.start_state.name);

    os << "Q = {";
    for (size_t i = 0; i < dfa.states.size(); i++) {
        os << dfa.states[i].name;
        if (i < dfa.states.size() - 1) os << ", ";
    }

    os << "}\nΣ = {";
    for (size_t i = 0; i < dfa.alphabet.size(); i++) {
        os << dfa.alphabet[i];
        if (i < dfa.alphabet.size() - 1) os << ", ";
    }

    os << "}\nδ = {";
    std::vector<std::string> transition_strings;
    for (const auto& [key, value] : dfa.transitions) {
        const auto& [origin, symbol] = key;
        transition_strings.push_back(std::format("{} --{}--> {}", origin.name, symbol, value.name));
    }
    for (size_t i = 0; i < transition_strings.size(); i++) {
        if (i > 0) os << std::string(5, ' ');
        os << transition_strings[i];
        if (i < transition_strings.size() - 1) os << ",\n";
    }

    os << "}\nF = {";
    for (size_t i = 0; i < dfa.final_states.size(); i++) {
        os << dfa.final_states[i].name;
        if (i < dfa.final_states.size() - 1) os << ", ";
    }
    os << "}\n";

    return os;
}


DFA::DFA(std::string n) : name(std::move(n)) {}

void DFA::add_state(const State& state) {
    if (is_built) throw DFAError::dfa_already_built();
    if (find_state(state).has_value()) throw DFAError::state_already_exists(state);
    states.push_back(state);
}

void DFA::add_symbol(char symbol) {
    if (is_built) throw DFAError::dfa_already_built();
    if (find_symbol(symbol).has_value()) throw DFAError::symbol_already_exists(symbol);
    alphabet.push_back(symbol);
}

void DFA::add_transition(const State& origin, char symbol, const State& destination) {
    if (is_built) throw DFAError::dfa_already_built();
    if (!find_symbol(symbol).has_value()) throw DFAError::symbol_not_found(symbol);
    if (!find_state(origin).has_value()) throw DFAError::state_not_found(origin);
    if (!find_state(destination).has_value()) throw DFAError::state_not_found(destination);
    if (transitions.find({origin, symbol}) != transitions.end()) throw DFAError::transition_already_exists(origin, symbol);
    transitions[{origin, symbol}] = destination;
}

void DFA::set_start_state(const State& start) {
    if (is_built) throw DFAError::dfa_already_built();
    if (start_state) throw DFAError::start_state_already_exists();
    if (!find_state(start).has_value()) throw DFAError::state_not_found(start);
    start_state = start;
}

void DFA::add_final_state(const State& final) {
    if (is_built) throw DFAError::dfa_already_built();
    if (!find_state(final).has_value()) throw DFAError::state_not_found(final);
    final_states.push_back(final);
}

std::vector<State> DFA::find_trap_states() {
    if (!is_built) throw DFAError::dfa_not_built();

    auto co_reachable = find_co_reachable_states();
    std::vector<State> trap_states;

    for (const auto& state : states) {
        if (!co_reachable.count(state)) trap_states.push_back(state);
    }

    return trap_states;
}

std::vector<State> DFA::find_unreachable_states() {
    if (!is_built) throw DFAError::dfa_not_built();

    auto reachable = find_reachable_states();
    std::vector<State> unreachable;

    for (const auto& state : states) {
        if (!reachable.count(state)) unreachable.push_back(state);
    }

    return unreachable;
}

bool DFA::is_empty() {
    if (!is_built) throw DFAError::dfa_not_built();

    auto reachable = find_reachable_states();
    auto co_reachable = find_co_reachable_states();

    for (const auto& s : reachable) {
        if (co_reachable.count(s)) return false;
    }

    return true;
}

void DFA::build() {
    if (name.empty()) throw DFAError::dfa_name_not_set();
    if (states.empty()) throw DFAError::dfa_no_states();
    if (final_states.empty()) throw DFAError::dfa_no_final_states();
    if (!start_state) throw DFAError::dfa_no_start_state();
    if (alphabet.empty()) throw DFAError::dfa_no_alphabet();

    const State dead{"DEAD"};
    bool needs_DEAD = false;

    for (char symbol : alphabet) {
        for (const auto& state : states) {
            if (transitions.find({state, symbol}) == transitions.end()) {
                needs_DEAD = true;
                transitions[{state, symbol}] = dead;
            }
        }
    }

    if (needs_DEAD) {
        for (char symbol : alphabet) transitions[{dead, symbol}] = dead;
        states.push_back(dead);
    }

    is_built = true;
}

void DFA::simulate(const std::string& input) {
    if (!is_built) throw DFAError::dfa_not_built();

    std::cout << std::format("Input string: {}.\n", input);

    auto current = start_state;
    std::cout << std::format("Start at state {}.\n", current.name);

    for (char symbol : input) {
        auto it = transitions.find({current, symbol});

        if (it == transitions.end()) throw DFAError::symbol_not_found(symbol);

        std::cout << std::format("Move from {} to {} via '{}'.\n", current.name, it->second.name, symbol);

        if (it->second == State{"DEAD"}) {
            std::cout << "Execution halted early.\n";
            std::cout << "Result: Rejected.\n";
            return;
        }

        current = it->second;
    }

    if (std::find(final_states.begin(), final_states.end(), current) == final_states.end()) {
        std::cout << "Result: Rejected.\n";
    } else {
        std::cout << std::format("Halted at state {}.\n", current.name);
        std::cout << "Result: Accepted.\n";
    }
}
