#ifndef RVARAGO_KITTEN_MONAD_H
#define RVARAGO_KITTEN_MONAD_H

#include <type_traits>

namespace rvarago::kitten {

    /**
    * A monad is an abstraction that allows to be mapped over where the mapping function itself returns a monad.
    *
    * Given a monad ma: M[A] and a function f: A -> M[B]
    *  It uses f to map over over ma, flat the return type, and then return a new monad mb: M[B].
    *
    * Note that if we had done as we do for functors, i.e. to do map(ma, f), we'd have a M[M[B]] that should then be flattened.
    * but bind does both the mapping and then the flattening.
    *
    * Laws:
    *
    * - Left identity: return a >>= f == f a
    * - Right identity: m >>= return == m
    * - Associativity: (m >>= f) >>= g == m >>= (\x -> f x >>= g)
    */
    template <template <typename ...> typename M, typename = void>
    struct monad;

    namespace traits {
        template <template <typename ...> typename, typename = void>
        struct is_monad : std::false_type{};

        template <template <typename ...> typename M>
        inline constexpr bool is_monad_v = is_monad<M>::value;
    }

    /**
     * Lifts a value of type A into a monad M[A].
     *
     * @param value the type parameter that defines the type of the element wrapped by the monad
     * @param tail eventual remaining type parameters used by the monad, considered as implementation detail
     * @return a new monad m: M[A] that wraps the contained value of type A
     */
    template <template <typename ...> typename M, typename A>
    constexpr decltype(auto) wrap(A&& value) {
        static_assert(traits::is_monad_v<M>, "type constructor M does not have a monad instance");
        return monad<M>::wrap(std::forward<A>(value));
    }

    /**
     * Forwards to the monad implementation.
     *
     * @param input a monad ma: M[A]
     * @param f a function A -> M[B] that maps over the value wrapped inside ma to produce a new monad mb_temp: M[B]
     * @return a new monad mb: M[B] resulting from applying f over the wrapped value inside ma and then flattening the result
     */
    template <typename UnaryFunction, template <typename...> typename M, typename A>
    constexpr decltype(auto) bind(M<A> const& input, UnaryFunction f) {
        static_assert(traits::is_monad_v<M>, "type constructor M does not have a monad instance");
        return monad<M>::bind(input, f);
    }

    /**
     * Infix version of bind.
     */

    template <typename UnaryFunction, template <typename...> typename M, typename A>
    constexpr decltype(auto) operator>>=(M<A> const& input, UnaryFunction f) {
        return bind(input, f);
    }

}

#endif
