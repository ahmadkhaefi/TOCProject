#pragma once

#include <stdexcept>
#include <format>
#include "state.h++"

using rte = std::runtime_error;

constexpr char LAMBDA = '\0';

struct AutomatonError {
    static rte state_not_found(const State& state) {
        return rte(std::format("State {} does not exist.", state.name));
    }
    static rte state_already_exists(const State& state) {
        return rte(std::format("State {} already exists.", state.name));
    }
    static rte start_state_already_exists(const std::string& machine) {
        return rte(std::format("{} already contains a start state.", machine));
    }
    static rte symbol_already_exists(char symbol) {
        return rte(std::format("Symbol '{}' already exists.", symbol));
    }
    static rte name_not_set(const std::string& machine) {
        return rte(std::format("{} must have a name.", machine));
    }
    static rte no_states(const std::string& machine) {
        return rte(std::format("{} must have at least one state.", machine));
    }
    static rte no_final_states(const std::string& machine) {
        return rte(std::format("{} must have at least one final state.", machine));
    }
    static rte no_start_state(const std::string& machine) {
        return rte(std::format("{} must have a start state.", machine));
    }
    static rte not_built(const std::string& machine) {
        return rte(std::format("{} must be built before it can be used.", machine));
    }
    static rte already_built(const std::string& machine) {
        return rte(std::format("{} is already built and cannot be modified.", machine));
    }
};

struct DFAError : AutomatonError {
    using AutomatonError::state_not_found;
    using AutomatonError::state_already_exists;
    using AutomatonError::symbol_already_exists;

    static rte start_state_already_exists() {
        return AutomatonError::start_state_already_exists("DFA");
    }
    static rte transition_already_exists(const State& origin, char symbol) {
        return rte(std::format("Transition ({}, {}) already exists.", origin.name, symbol));
    }
    static rte symbol_not_found(char symbol) {
        return rte(std::format("Symbol {} does not exist in the alphabet.", symbol));
    }
    static rte dfa_name_not_set() {
        return AutomatonError::name_not_set("DFA");
    }
    static rte dfa_no_states() {
        return AutomatonError::no_states("DFA");
    }
    static rte dfa_no_final_states() {
        return AutomatonError::no_final_states("DFA");
    }
    static rte dfa_no_start_state() {
        return AutomatonError::no_start_state("DFA");
    }
    static rte dfa_no_alphabet() {
        return rte("DFA must have at least one symbol in the alphabet.");
    }
    static rte dfa_not_built() {
        return AutomatonError::not_built("DFA");
    }
    static rte dfa_already_built() {
        return AutomatonError::already_built("DFA");
    }
    static rte execution_halted_early(const std::string& reason) {
        return rte(std::format("Execution of DFA halted early because {}.", reason));
    }
};

struct DPDAError : AutomatonError {
    using AutomatonError::state_not_found;
    using AutomatonError::state_already_exists;
    using AutomatonError::symbol_already_exists;

    static rte start_state_already_exists() {
        return AutomatonError::start_state_already_exists("DPDA");
    }
    static rte symbol_not_found(char symbol) {
        return rte(std::format("Symbol '{}' does not exist in the input alphabet.", symbol));
    }
    static rte stack_symbol_not_found(char symbol) {
        return rte(std::format("Symbol '{}' does not exist in the stack alphabet.", symbol));
    }
    static rte stack_symbol_already_exists(char symbol) {
        return rte(std::format("Symbol '{}' already exists in the stack alphabet.", symbol));
    }
    static rte transition_already_exists(const State& origin, char input, char stack_top) {
        return rte(std::format("Transition ({}, {}, {}) already exists.", origin.name,
            input == LAMBDA ? "lambda" : std::string(1, input),
            stack_top));
    }
    static rte nondeterministic_transition(const State& origin, char stack_top) {
        return rte(std::format(
            "DPDA is nondeterministic: state {} with stack top '{}' has both a lambda "
            "transition and a transition on an input symbol.", origin.name, stack_top));
    }
    static rte initial_stack_symbol_not_set() {
        return rte("DPDA must have an initial stack symbol.");
    }
    static rte initial_stack_symbol_not_in_alphabet() {
        return rte("Initial stack symbol must be a member of the stack alphabet.");
    }
    static rte invalid_acceptance_mode(const std::string& mode) {
        return rte(std::format("Invalid acceptance mode '{}'. Must be 'final' or 'empty'.", mode));
    }
    static rte dpda_name_not_set() {
        return AutomatonError::name_not_set("DPDA");
    }
    static rte dpda_no_states() {
        return AutomatonError::no_states("DPDA");
    }
    static rte dpda_no_final_states() {
        return rte("DPDA must have at least one final state when using final-state acceptance.");
    }
    static rte dpda_no_start_state() {
        return AutomatonError::no_start_state("DPDA");
    }
    static rte dpda_no_input_alphabet() {
        return rte("DPDA must have at least one symbol in the input alphabet.");
    }
    static rte dpda_no_stack_alphabet() {
        return rte("DPDA must have at least one symbol in the stack alphabet.");
    }
    static rte dpda_not_built() {
        return AutomatonError::not_built("DPDA");
    }
    static rte dpda_already_built() {
        return AutomatonError::already_built("DPDA");
    }
};
