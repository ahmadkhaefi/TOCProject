#pragma once

#include <string>
#include <ostream>

using rte = std::runtime_error;

struct State {
    std::string name;

    explicit operator bool() const {return !name.empty();}
    bool operator!() const {return name.empty();}
    bool operator<(const State& other) const {return name < other.name;}
    bool operator>(const State& other) const {return name > other.name;}
    bool operator==(const State& other) const {return name == other.name;}
    bool operator!=(const State& other) const {return name != other.name;}
    friend std::ostream& operator<<(std::ostream &os, const State& state) {
        os << state.name;

        return os;
    }
};
