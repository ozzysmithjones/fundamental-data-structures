#include <iostream>
#include "fundamental.hpp"
#include <memory>

int main(int, char**) {
    dynamic_array<int> arr;
    arr.preallocate_atleast(10);
    for (int i = 0; i < 10; ++i) {
        arr.append(i);
    }
    if (arr.is_empty()) {
        std::cout << "Array is empty\n";
    } else {
        std::cout << "Array is not empty, contents:\n";
        for (size_t i = 0; i < arr.get_count(); ++i) {
            std::cout << arr.element_unsafe(i) << " ";
        }
        std::cout << "\n";
    }
}
