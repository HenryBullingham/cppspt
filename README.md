# C++ Standard Parameter Types

A set of input parameter types that simplify the rules for managing inputs to functions.
The types are uninit, in, inout, out, move and forward, based on Herb Sutter's 2020 CppCon talk.

## Usage:

``` C++

// For uninitialized types, they are constructed on first assignment

void test_uninit()
{
    cppspt::uninit<std::string> my_string; //No constructor called!

    my_string = std::string("Hello"); //Here my_string will be move constructed

    my_string = std::string("World"); //My string will be copy-assigned

    //Here, my_string is destroyed
}

// For uninitialized types, they are constructed on first assignment

void test_uninit()
{
    cppspt::uninit<std::string> my_string; //No constructor called!

    my_string = std::string("Hello"); //Here my_string will be move constructed

    my_string = std::string("World"); //My string will be copy-assigned

    //Here, my_string is destroyed
}

//For in types, the can capture moves or const refs



void add_to_dict(cppspt::in<std::string> key, cppspt::in<std::string> value)
{
    //Dictionary stuff
    ...
    std::string inplace_key = std::move(key);
    std::string inplace_val = std::move(val);
}

void test_in()
{
    std::string by_ref = "World";

    //Will do a move construction for Hello and a copy construction for by_ref
    add_to_dict(std::string("Hello"), by_ref);
}

//For out types, can be either a ref or uninit

void write_to_out(cppspt::out<std::string> the_out)
{
    the_out = "Hello World";
}

void test_out()
{
    std::string normal;
    cppspt::uninit<std::string> uninit;

    write_to_out(normal); //Will call assign
    write_to_out(uninit); //Will call constructor
}

//For inout types, must be a non-const ref
void test_inout(inout<std::string> str)
{
    std::string copied = str;
    str = "EMPTY";
}

//For move types, move onle
void test_move(move<std::string> param)
{
    std::string held = std::move(param);
}

//For forward types, captures forwarding references
template<typename T>
void do_something(forward<T> ref)
{
    std::string held = std::forward(ref);
}

void test_forward
{
    std::string the_ref = "Hello";

    do_something(the_ref); //Will call with regular ref

    do_something(std::string("World")); //Will call with move ref
}

```

## Contributing
Pull requests are welcome. For major changes, please open an issue first to discuss what you would like to change.

Please make sure to update tests as appropriate.

## License
[MIT](https://choosealicense.com/licenses/mit/)