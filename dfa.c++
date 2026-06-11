#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <map>
#include <set>
#include <unordered_set>
#include <algorithm>
#include <initializer_list>
#include <queue>
#include <format>

using rte = std::runtime_error;

struct State {
    std::string name;

    explicit operator bool() const {
        return !name.empty();
    }
    bool operator!() const {
        return name.empty();
    }
    bool operator<(const State& other) const {
        return name < other.name;
    }
    bool operator>(const State& other) const {
        return name > other.name;
    }
    bool operator==(const State& other) const {
        return name == other.name;
    }
    bool operator!=(const State& other) const {
        return name != other.name;
    }
    friend std::ostream& operator<<(std::ostream &os, const State& state) {
        os << state.name;

        return os;
    }
};

struct DFAError {
    static rte state_not_found(const State& state) {
        return rte(std::format("State {} does not exist.", state.name));
    }
    static rte state_already_exists(const State& state) {
        return rte(std::format("State {} already exists.", state.name));
    }
    static rte start_state_already_exists() {
        return rte("DFA already contains a start state.");
    }
    static rte transition_already_exists(const State& origin, char symbol) {
        return rte(std::format("Transition ({}, {}) already exists.", origin.name, symbol));
    }
    static rte symbol_not_found(char symbol) {
        return rte(std::format("Symbol {} does not exist in the alphabet.", symbol));
    }
    static rte symbol_already_exists(char symbol) {
        return rte(std::format("Symbol {} already exists.", symbol));
    }
    static rte dfa_name_not_set() {
        return rte("DFA must have a name.");
    }
    static rte dfa_no_states() {
        return rte("DFA must have at least one state.");
    }
    static rte dfa_no_final_states() {
        return rte("DFA must have at least one final state.");
    }
    static rte dfa_no_start_state() {
        return rte("DFA must have a start state.");
    }
    static rte dfa_no_alphabet() {
        return rte("DFA must have at least one symbol in the alphabet.");
    }
    static rte dfa_not_built() {
        return rte("DFA must be built before it can be used.");
    }
    static rte dfa_already_built() {
        return rte("DFA is already built and cannot be modified.");
    }
    static rte execution_halted_early(const std::string& reason) {
        return rte(std::format("Execution of DFA halted early because {}.", reason));
    }
};

struct DFA {
    private:
    std::string name;

    bool is_built = false;

    std::vector<State> states;
    std::vector<char> alphabet;
    std::map<std::pair<State, char>, State> transitions;
    State start_state;
    std::vector<State> final_states;

    std::optional<std::vector<State>::iterator> find_state(const State& state) {
        auto it = std::find(states.begin(), states.end(), state);

        if (it == states.end()) return std::nullopt;
        
        return it;
    }
    std::optional<std::vector<char>::iterator> find_symbol(char symbol) {
        auto it = std::find(alphabet.begin(), alphabet.end(), symbol);

        if (it == alphabet.end()) return std::nullopt;

        return it;
    }
    std::set<State> find_reachable_states() {
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
    std::set<State> find_co_reachable_states() {
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
    friend std::ostream& operator<<(std::ostream& os, const DFA& dfa) {
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
    public:
    DFA (std::string n) : name(n) {};
    void add_state(const State &state) {
        if (is_built) throw DFAError::dfa_already_built();
        if (find_state(state).has_value()) throw DFAError::state_already_exists(state);

        states.push_back(state);
    }
    void add_symbol(char symbol) {
        if (is_built) throw DFAError::dfa_already_built();
        if (find_symbol(symbol).has_value()) throw DFAError::symbol_already_exists(symbol);

        alphabet.push_back(symbol);
    }
    void add_transition(const State& origin, char symbol, const State& destination) {
        if (is_built) throw DFAError::dfa_already_built();
        if (!find_symbol(symbol).has_value()) throw DFAError::symbol_not_found(symbol);
        if (!find_state(origin).has_value()) throw DFAError::state_not_found(origin);
        if (!find_state(destination).has_value()) throw DFAError::state_not_found(destination);
        if (transitions.find({origin, symbol}) != transitions.end()) throw DFAError::transition_already_exists(origin, symbol);

        transitions[{origin, symbol}] = destination;
    }
    void set_start_state(const State &start) {
        if (is_built) throw DFAError::dfa_already_built();
        if (start_state) throw DFAError::start_state_already_exists();
        if (!find_state(start).has_value()) throw DFAError::state_not_found(start);

        start_state = start;
    }
    void add_final_state(const State &final) {
        if (is_built) throw DFAError::dfa_already_built();
        if (!find_state(final).has_value()) throw DFAError::state_not_found(final);

        final_states.push_back(final);
    }
    std::vector<State> find_trap_states() {
        if (!is_built) throw DFAError::dfa_not_built();

        auto co_reachable = find_co_reachable_states();
        std::vector<State> trap_states;

        for (const auto& state : states) {
            if (!co_reachable.count(state)) trap_states.push_back(state);
        }

        return trap_states;
    }
    std::vector<State> find_unreachable_states() {
        if (!is_built) throw DFAError::dfa_not_built();

        auto reachable = find_reachable_states();
        std::vector<State> unreachable;

        for (const auto& state : states) {
            if (!reachable.count(state)) unreachable.push_back(state);
        }

        return unreachable;
    }
    bool is_empty() {
        if (!is_built) throw DFAError::dfa_not_built();

        auto reachable = find_reachable_states();
        auto co_reachable = find_co_reachable_states();

        for (const auto& s : reachable) {
            if (co_reachable.count(s)) return false;
        }

        return true;
    }
    void build() {
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
    void simulate(const std::string& input) {
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
            };

            current = it->second;
        }

        if (std::find(final_states.begin(), final_states.end(), current) == final_states.end()) {
            std::cout << "Result: Rejected.\n";
        } else {
            std::cout << std::format("Halted at state {}.\n", current.name);
            std::cout << "Result: Accepted.\n";
        }
    }
};

int main() {
    DFA dfa("M");

    std::string line;

    // STATES
    std::cout << "STATES: ";

    if (std::getline(std::cin, line)) {
        std::stringstream ss(line);
        std::string state;

        while (ss >> state) dfa.add_state(State(state));
    }

    // ALPHABET
    std::cout << "ALPHABET: ";

    if (std::getline(std::cin, line)) {
        std::stringstream ss(line);
        std::string symbol;

        while (ss >> symbol) dfa.add_symbol(symbol[0]);
    }

    // START STATE
    std::cout << "START STATE: ";

    if (std::getline(std::cin, line)) {
        std::stringstream ss(line);
        std::string start;

        ss >> start;

        dfa.set_start_state(State(start));
    }

    // FINAL STATES
    std::cout << "FINAL STATES: ";

    if (std::getline(std::cin, line)) {
        std::stringstream ss(line);
        std::string state;

        while (ss >> state) dfa.add_final_state(State(state));
    }

    // TRANSITIONS
    std::cout << "NUMBER OF TRANSITIONS: ";

    int number_of_transitions;

    if (std::getline(std::cin, line)) {
        std::stringstream ss(line);

        ss >> number_of_transitions;
    }

    // int maximum_number_of_transitions = dfa.size() * alphabet.size();

    // if (number_of_transitions > maximum_number_of_transitions) {
    //     throw std::runtime_error(std::format("Number of transitions must not exceed {}.", maximum_number_of_transitions));
    // }

    for (size_t i = 0; i < number_of_transitions; i++) {
        if (std::getline(std::cin, line)) {
            std::stringstream ss(line);
            std::string origin, symbol, destination;

            ss >> origin >> symbol >> destination;
            dfa.add_transition(State(origin), symbol[0], State(destination));
        }
    }

    dfa.build();

    // STRINGS
    std::cout << "NUMBER OF STRINGS: ";

    int number_of_strings;

    if (std::getline(std::cin, line)) {
        std::stringstream ss(line);

        ss >> number_of_strings;
    }

    for (size_t i = 0; i < number_of_strings; i++) {
        if (std::getline(std::cin, line)) {
            std::stringstream ss(line);
            std::string str;

            ss >> str;
            
            dfa.simulate(str);
        }
    }
}
