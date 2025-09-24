# Fundamental Data Structures
Single header file of fundamental data structures useful in C++ projects. 

# Rationale

I wanted more generic data-structures not provided by the standard library (capped_array, enum_array, enum_bitset). I also made more readable and efficient alternatives to the standard library data-structures. Some differences to the standard library:

- Allocator is not templatized. It is possible to re-use a single allocator for different types.
- No exceptions. No overloaded indexing operator for dynamic_array and other types. Instead the API is designed such that the programmer must consider that an index out of bounds is a possibility. Methods like `element_or` provide a way to access elements with a fallback alternative should the indexing fail. 
- More intuitive names for things. For instance, the equivalent of `std::vector` is called `dynamic_array`. I do not know why programmers keep calling this data structure a `dynamic_array` in parlance and never name it that in the programming language.
- Dynamic array assumes that your types are trivially relocatable, so that it can grow more efficiently. 
- More useful functions like a `contains` function for dynamic_array to see if it contains an element. 

# Exception Safety

The C++ data-structures that I've made should still be exception safe, but if you see anything that isn't feel free to make a pull-request. I've used static_asserts to ensure that types are `nothrow_move_constructible` so moves can be done safely (normally it's fairly difficult to reverse out of moving multiple items efficiently because arrays can be left in a partial state if you are not careful). 
