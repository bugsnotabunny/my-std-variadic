Let's look at this function:
```cpp
void match(std::string input)
{
    if (input == "FOO") {
        foo();
    } else if (input == "BAR") {
        bar();   
    } else if (input == "LOL") {
        lol();
    } else if (input == "DEE") {
        dee();
    }
}
```
There are a couple of problems. First of all, this function is spacious and is somewhat hard to read. This could be improved easily:
```cpp
void match(std::string input)
{
    constexpr static std::map<std::string, void(*)()> opts{
        {"FOO", foo},
        {"BAR", bar},
        {"LOL", lol},
        {"DEE", dee}
    };
    (*opts.at(input))()
}
```
This is now much easier for a reader to recognize which option does what. Moreover, we have decreased amount of string comparisons (an expensive operation if you ask me). And the most beautyful part is that now adding a new route is not all about writi actual code, but about changing some predefined constants, which is great, if we want even the most unexpierienced programmer to be able to adjust our system (for example, as I know, some window managers are configured through modification of actual source code).

However, we have sacrificed our performance for this bit of syntax sugar. This adds a heap allocation. Also, std::map is a binary tree, and each of it's nodes is accessible through pointer, which adds extra overhead.

Another thing we lack in this approach is flexibility. What if we don't necessarily want our map values to be of the very same type? The usual OOP answer to that is polymorphysm:

```cpp
void match(std::string input)
{
    constexpr static std::map<std::string, std::function<void()>> opts{
        {"FOO", foo},
        {"BAR", bar_but_lambda},
        {"LOL", lol_but_functional_object},
        {"DEE", dee_but_std_bind}
    };
    opts.at(input)()
}
```
Now we have dropped performance EVEN MORE. Abstraction through polymorphism comes at cost. All of your types have to become 8 bytes bigger (often more due to aligning), because of vtable pointer added. EACH call to an abstract method results in dereferencing pointer to vtable, and then a pointer to an actual method. This is crazy.

Now let's dive into my proposal:
```cpp
void match(std::string input)
{
    constexpr static auto opts = stct::make_map(
        Tuple{std::string("FOO"), foo},
        Tuple{std::string("BAR"), bar_but_lambda},
        Tuple{std::string("LOL"), lol_but_functional_object},
        Tuple{std::string("DEE"), dee_but_std_bind}
    );
    visitors::call_at(input);
}
```

What is the difference? Firstly, all of that beauty is located directly on the stack, which means no allocation, and no dynamic pointer dereferencing (which makes it easier for compiler to optimize). Secondly, functional objects are still of different types, but now there is no abstraction through runtime pointer-powered polymorphysm.

The performance increase compared to standard map is descent (you can run benchmarks to make sure). And we still have got a much more idiomatic and much more easy-to-alter code than a bunch of ifs were.

Maybe there is a way to make template deduction smarter, so there won't be a need to write endless Tuple{} and std::string{}. However, this already is a sort of proof of concept for me.