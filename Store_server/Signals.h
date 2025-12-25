#pragma once
#include <functional>
#include <vector>
#include <map>
#include "StoreEnt.h" 

template<typename... Args>
class Signal {
private:
    std::vector<std::function<void(Args...)>> slots;

public:
    void connect(std::function<void(Args...)> slot) {
        slots.push_back(slot);
    }

    void emit(Args... args) {
        for (auto& slot : slots) {
            slot(args...);
        }
    }
};

class GlobalSignals {
public:
    static Signal<Product*, int>& productAddedToCheck() {
        static Signal<Product*, int> signal;
        return signal;
    }

    static Signal<Product*, int>& productRemovedFromCheck() {
        static Signal<Product*, int> signal;
        return signal;
    }
};