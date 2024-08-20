# state_machine

c++ template DSL style impl state_machine

# features

- header only
- C++11 or later
- formly defined state machine in DSL style
- decouple state machine define and implementation

# usage
1- include state_machine.h in your source file
```c++
#include "state_machine.h" 

```
2- define all states and trigger events for transitions,
we use a struct/class to define an event, so it can bring data/parameters for the transition
```c++
enum class State {
    S1,
    S2,
}

struct E1 {
};

struct E2 {
};
```
3- define the state machine derived from state_machine<T,S>
```c++
struct MyFsm : state_machine<MySM, State::S1> {
};
```

the second param is the init state of your state machine.

4- add transtion handling functions for each event and state pair, the virtual functions `on_undefed_act` ,`on_default_act` will be invoked if no transition is defined for the current state and event.
```c++
    void do_pay( int state_, const put& evt_ ) { std::cout << "do pay" << std::endl; }
    void do_pack( int state_, const paied& evt_ ) { std::cout << "do pack" << std::endl; }
    void do_reload( int state_, const cont& evt_ ) { std::cout << "do reload" << std::endl; }

```

define the transition table at the end of the class definition.
```c++
    TRANSITIONS( _T( st::init, put, st::payment, &order_processor::do_pay ),
                 _T( st::payment, paied, st::packing ),  //!! will use default
                 _T( st::shipped, cont, st::init, &order_processor::do_reload ) );
```

# example and unittest

example.cpp

# author

wechat:371536435 email:yzbit@outlook.com