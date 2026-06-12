#pragma once

#include <stdexcept>
#include <format>
#include "state.h++"

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
