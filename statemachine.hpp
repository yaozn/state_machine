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
#ifndef FDD652EB_9E3A_42F5_9639_B0BF256A9FEE
#define FDD652EB_9E3A_42F5_9639_B0BF256A9FEE

#include <iostream>
#include <stdarg.h>

#define __meta_call( ... ) typename __VA_ARGS__::result

namespace yy {
namespace details {
    template <int N>
    struct int_type {
        enum { value = N };
    };

    template <bool B>
    struct bool_type {
        enum { value = B };
    };

    using true_type  = bool_type<true>;
    using false_type = bool_type<false>;

    template <typename N, typename V>
    struct is_equal {
        using result = false_type;
    };

    template <typename N>
    struct is_equal<N, N> {
        using result = true_type;
    };

    template <typename F, typename T, typename E>
    struct if_else_then;

    template <typename T, typename E>
    struct if_else_then<true_type, T, E> {
        using result = T;
    };

    template <typename T, typename E>
    struct if_else_then<false_type, T, E> {
        using result = E;
    };

    struct null_type;
    template <typename H, typename T>
    struct type_node {
        using Head = H;
        using Tail = T;
    };

    template <typename H, typename... T>
    struct type_list {
        using result = type_node<H, typename type_list<T...>::result>;
    };

    template <typename H>
    struct type_list<H> {
        using result = type_node<H, null_type>;
    };

    template <typename L, template <typename> class Pred>
    struct filter;

    template <typename H, typename T, template <typename> class Pred>
    struct filter<type_node<H, T>, Pred> {
        using result = __meta_call( if_else_then<typename Pred<H>::result,
                                                 type_node<H, typename filter<T, Pred>::result>,
                                                 typename filter<T, Pred>::result> );
    };

    template <template <typename T> class Pred>
    struct filter<null_type, Pred> {
        using result = null_type;
    };

    template <typename TL, typename T, template <typename Acc, typename U> class Func>
    struct fold;

    template <typename T, template <typename Acc, typename U> class Func>
    struct fold<null_type, T, Func> {
        using result = T;
    };

    template <typename H, typename T, typename E, template <typename Acc, typename U> class Func>
    struct fold<type_node<H, T>, E, Func> {
        using result = __meta_call( Func<typename fold<T, E, Func>::result, H> );
    };

    template <typename Next, typename Transition>
    struct event_dispatcher {
        using fsm   = typename Transition::fsm;
        using event = typename Transition::event;

        static int dispatch( fsm& fsm_, int state_, const event& e_ ) {
            if ( state_ == Transition::current ) {
                return Transition::execute( fsm_, state_, e_ );
            }
            else {
                return Next::dispatch( fsm_, state_, e_ );
            }

            return 0;
        }

        using result = event_dispatcher;
    };

    struct default_dispatcher {
        template <typename Fsm, typename Event>
        static int dispatch( Fsm& fsm_, int state_, const Event& e ) {
            fsm_.on_undef_act( state_, &e );
            return 0;
        }
    };

    template <typename E, typename T>
    struct transition_matcher {
        using result = __meta_call( yy::details::is_equal<E, typename T::event> );
    };

    template <typename E>
    struct dispatcher_filter {
        template <typename Tr>
        using result = transition_matcher<E, Tr>;
    };
}  // namespace details

template <typename Impl, int Init>
struct state_machine {
    virtual ~state_machine() {}

    template <typename E>
    int transit( const E& e_ ) {
        using filted      = __meta_call( yy::details::filter<typename Impl::__transitions, yy::details::dispatcher_filter<E>::template result> );
        using Dispatchers = __meta_call( yy::details::fold<filted, yy::details::default_dispatcher, yy::details::event_dispatcher> );
        return Dispatchers::dispatch( *static_cast<Impl*>( this ), _current, e_ );
    }

    virtual void on_undef_act( int curr_state_, const void* evt_ ) {
        std::cout << "undefined: state=" << curr_state_ << std::endl;
    }

    virtual void on_default_act( int curr_state_, int next_state_, const void* evt_ ) {
        std::cout << "default: state=" << curr_state_ << std::endl;
    }

protected:
    template <int S, typename E, int N, void ( Impl::*Action )( int s_, const E& e_ ) = nullptr>
    struct transition_item {
        enum {
            current = S,
            next    = N
        };

        using event = E;
        using fsm   = Impl;

        static int execute( fsm& fsm_, int s_, const E& e_ ) {
            if ( nullptr == Action ) {
                fsm_.on_default_act( s_, ( int )transition_item::next, &e_ );
            }
            else {
                ( &fsm_->*Action )( s_, e_ );
            }

            fsm_._current = transition_item::next;
            return fsm_._current;
        }
    };

protected:
    int _current = Init;
};
}  // namespace yy

#define __is_equal( ... ) __meta_call( yy::details::is_equal<__VA_ARGS__> )
#define __is_equal_v( ... ) yy::details::is_equal<__VA_ARGS__>::result::value

#define __if_else( ... ) __meta_call( yy::details::if_else_then<__VA_ARGS__> )
#define __typelist( ... ) __meta_call( yy::details::type_list<__VA_ARGS__> )
#define __filter( ... ) __meta_call( yy::details::filter<__VA_ARGS__> )
#define __fold( ... ) __meta_call( yy::details::fold<__VA_ARGS__> )
#define __typelist( ... ) __meta_call( yy::details::type_list<__VA_ARGS__> )
#define __table __typelist
#define __tr( ... ) transition_item<__VA_ARGS__>

#define TRANSITIONS( ... ) using __transitions = __table( __VA_ARGS__ );
#define _T( ... ) __tr( __VA_ARGS__ )

#endif /* FDD652EB_9E3A_42F5_9639_B0BF256A9FEE */
