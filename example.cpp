/*
BSD 3-Clause License

Copyright (c) 2024, YaoZinan

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this
   list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its
   contributors may be used to endorse or promote products derived from
   this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

wechat:371536435 email:yzbit@outlook.com
*/

#include <cassert>

#include "statemachine.hpp"

enum st {
    init,
    payment,
    packing,
    shipped
};

/*
* 事件最好是类型,方便带参数
enum Evt {
    Put,
    Paied,
    Packed,
    Cont
};
*/

struct event {};
struct put : event {};
struct paied : event {};
struct packed : event {};
struct cont : event {};

struct order_processor : yy::state_machine<order_processor, st::init> {
    //-unexpected event + state
    virtual void on_undef_act( int curr_state_, const void* evt_ ) {
        std::cout << "!!undefined: state=" << curr_state_ << std::endl;
    }

    //-default event  and transition handler
    virtual void on_default_act( int curr_state_, int next_state_, const void* evt_ ) {
        std::cout << "!!default: state=" << curr_state_ << std::endl;
    }

    //-action handlers
    void do_pay( int state_, const event& evt_ ) { std::cout << "do pay" << std::endl; }
    void do_pack( int state_, const paied& evt_ ) { std::cout << "do pack" << std::endl; }
    void do_reload( int state_, const cont& evt_ ) { std::cout << "do reload" << std::endl; }

    //-transition table
    TRANSITIONS( _T( st::init, event, st::payment, &order_processor::do_pay ),
                 _T( st::payment, paied, st::packing ),  //!! will use default
                 _T( st::shipped, cont, st::init, &order_processor::do_reload ) );
};

int main() {
    // case1- statemachine
    order_processor op;

    auto trans = 0;
    trans      = op.transit<event>( put() );

    assert( trans == st::payment );  // call do pay
    trans = op.transit( paied() );
    assert( trans == st::packing );  // call defaultg
    trans = op.transit( cont() );
    assert( trans == st::init );

    // case2- meta functions
    //  isequal
    //  assert( __is_equal_v( int, int ) );
    using eq = __is_equal( int, int );
    //__is_equal( int, int )::value;
    // std::cout << __is_equal_v( int, int );
    // std::cout << eq::value << std::endl;

    assert( !__is_equal_v( int, char ) );

    // ifelse
    using t1 = __if_else( yy::details::false_type, int, char );
    using t2 = __if_else( yy::details::true_type, int, char );
    assert( __is_equal_v( t1, char ) );
    assert( __is_equal_v( t2, int ) );

    // list filter
    using list = __typelist( int, int, char, double );
    // using filter = __filter( list, Cond );
    // assert( __is_equal_v( filter, __typelist( int, int ) ) );

    // list fold

    return 0;
}
