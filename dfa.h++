#pragma once

#include <string>
#include <vector>
#include <map>
#include <set>
#include <optional>
#include <ostream>
#include "state.h++"
#include "error.h++"

struct DFA {
private:
    std::string name;
    bool is_built = false;
    std::vector<State> states;
    std::vector<char> alphabet;
    std::map<std::pair<State, char>, State> transitions;
    State start_state;
    std::vector<State> final_states;

    std::optional<std::vector<State>::iterator> find_state(const State&);
    std::optional<std::vector<char>::iterator> find_symbol(char);
    std::set<State> find_reachable_states();
    std::set<State> find_co_reachable_states();

public:
    explicit DFA(std::string n);

    void add_state(const State&);
    void add_symbol(char);
    void add_transition(const State& origin, char symbol, const State& dest);
    void set_start_state(const State&);
    void add_final_state(const State&);

    std::vector<State> find_trap_states();
    std::vector<State> find_unreachable_states();
    bool is_empty();

    void build();
    void simulate(const std::string& input);

    friend std::ostream& operator<<(std::ostream&, const DFA&);
};
