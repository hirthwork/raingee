/*
 * iscallable.hpp           -- nullary callable objects detection
 *
 * Copyright (C) 2012 Dmitry Potapov <potapov.d@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef __ISCALLABLE_HPP_2012_02_22__
#define __ISCALLABLE_HPP_2012_02_22__

#include <reinvented-wheels/enableif.hpp>

namespace NRaingee
{
    template <class>
    struct TIsReference
    {
        static const bool Value_ = false;
    };

    template <class TType>
    struct TIsReference<TType&>
    {
        static const bool Value_ = true;
    };

    template <class TType>
    struct TRemoveConst
    {
        typedef TType TType_;
    };

    template <class TType>
    struct TRemoveConst<const TType>
    {
        typedef TType TType_;
    };

    template <class TType>
    struct TRemoveConst<const TType&>
    {
        typedef TType& TType_;
    };

    template <class TType>
    struct TRemoveVolatile
    {
        typedef TType TType_;
    };

    template <class TType>
    struct TRemoveVolatile<volatile TType>
    {
        typedef TType TType_;
    };

    template <class TType>
    struct TRemoveVolatile<volatile TType&>
    {
        typedef TType& TType_;
    };

    template <class TType>
    struct TIsConst
    {
        static const bool Value_ = false;
    };

    template <class TType>
    struct TIsConst<const TType>
    {
        static const bool Value_ = true;
    };

    template <class TType>
    struct TIsConst<const TType&>
    {
        static const bool Value_ = true;
    };

    template <class TType>
    struct TIsVolatile
    {
        static const bool Value_ = false;
    };

    template <class TType>
    struct TIsVolatile<volatile TType>
    {
        static const bool Value_ = true;
    };

    template <class TType>
    struct TIsVolatile<volatile TType&>
    {
        static const bool Value_ = true;
    };

    template <class, class>
    struct TIsNullaryFunction
    {
        static const bool Value_ = false;
    };

    template <class TResult>
    struct TIsNullaryFunction<TResult (*)(), TResult>
    {
        static const bool Value_ = true;
    };

    template <bool, class TFunctor, class TResult>
    struct TIsNullaryFunctor
    {
        static const bool Value_ = false;
    };

    template <class T, T>
    class TTester;

    template <class TFunctor, class TResult>
    struct TIsNullaryFunctor<true, TFunctor, TResult>
    {
        template <class T>
        static char Test(TTester<TResult (T::*)(), &T::operator()>*);

        template <class T>
        static char Test(TTester<TResult (T::*)() const, &T::operator()>*);

        template <class T>
        static char Test(TTester<TResult (T::*)() volatile, &T::operator()>*);

        template <class T>
        static char Test(TTester<TResult (T::*)() const volatile,
            &T::operator()>*);

        typedef char TTwoChars[2];
        template <class>
        static TTwoChars& Test(...);

        static const bool Value_ = sizeof(Test<TFunctor>(0)) == 1;
    };

    template <bool, class TFunctor, class TResult>
    struct TIsNullaryConstFunctor
    {
        static const bool Value_ = false;
    };

    template <class TFunctor, class TResult>
    struct TIsNullaryConstFunctor<true, TFunctor, TResult>
    {
        template <class T>
        static char Test(TTester<TResult (T::*)() const, &T::operator()>*);

        template <class T>
        static char Test(TTester<TResult (T::*)() const volatile,
            &T::operator()>*);

        typedef char TTwoChars[2];
        template <class>
        static TTwoChars& Test(...);

        static const bool Value_ = sizeof(Test<TFunctor>(0)) == 1;
    };

    template <bool, class TFunctor, class TResult>
    struct TIsNullaryVolatileFunctor
    {
        static const bool Value_ = false;
    };

    template <class TFunctor, class TResult>
    struct TIsNullaryVolatileFunctor<true, TFunctor, TResult>
    {
        template <class T>
        static char Test(TTester<TResult (T::*)() volatile, &T::operator()>*);

        template <class T>
        static char Test(TTester<TResult (T::*)() const volatile,
            &T::operator()>*);

        typedef char TTwoChars[2];
        template <class>
        static TTwoChars& Test(...);

        static const bool Value_ = sizeof(Test<TFunctor>(0)) == 1;
    };

    template <bool, class TFunctor, class TResult>
    struct TIsNullaryConstVolatileFunctor
    {
        static const bool Value_ = false;
    };

    template <class TFunctor, class TResult>
    struct TIsNullaryConstVolatileFunctor<true, TFunctor, TResult>
    {
        template <class T>
        static char Test(TTester<TResult (T::*)() const volatile,
            &T::operator()>*);

        typedef char TTwoChars[2];
        template <class>
        static TTwoChars& Test(...);

        static const bool Value_ = sizeof(Test<TFunctor>(0)) == 1;
    };

    template <class TFunctor, class TResult>
    struct TIsNullaryCallableImpl
    {
        static const bool Value_ =
            TIsNullaryFunction<TFunctor, TResult>::Value_
            || TIsNullaryFunctor<!TIsConst<TFunctor>::Value_
                && !TIsVolatile<TFunctor>::Value_, TFunctor, TResult>::Value_
            || TIsNullaryConstFunctor<TIsConst<TFunctor>::Value_
                && !TIsVolatile<TFunctor>::Value_, TFunctor, TResult>::Value_
            || TIsNullaryVolatileFunctor<!TIsConst<TFunctor>::Value_
                && TIsVolatile<TFunctor>::Value_, TFunctor, TResult>::Value_
            || TIsNullaryConstVolatileFunctor<TIsConst<TFunctor>::Value_
                && TIsVolatile<TFunctor>::Value_, TFunctor, TResult>::Value_;
    };

    template <bool, class TFunctor, class TResult>
    struct TIsNullaryCVCallableImpl
    {
        static const bool Value_ =
            TIsNullaryCallableImpl<TFunctor, TResult>::Value_
            || TIsNullaryCallableImpl<TFunctor, const TResult>::Value_
            || TIsNullaryCallableImpl<TFunctor, volatile TResult>::Value_
            || TIsNullaryCallableImpl<TFunctor,
                const volatile TResult>::Value_;
    };

    template <class TFunctor, class TResult>
    struct TIsNullaryCVCallableImpl<false, TFunctor, TResult>
    {
        static const bool Value_ =
            TIsNullaryCVCallableImpl<true, TFunctor, TResult>::Value_
            || TIsNullaryCallableImpl<TFunctor, TResult&>::Value_
            || TIsNullaryCallableImpl<TFunctor, const TResult&>::Value_
            || TIsNullaryCallableImpl<TFunctor, volatile TResult&>::Value_
            || TIsNullaryCallableImpl<TFunctor,
                const volatile TResult&>::Value_;
    };

    template <class TFunctor, class TResult>
    struct TIsNullaryCallableCVSelectorImpl
    {
        static const bool Value_ =
            TIsNullaryCVCallableImpl<TIsReference<TResult>::Value_,
            TFunctor, TResult>::Value_;
    };

    template <class TFunctor, class TResult>
    struct TIsNullaryCallable
    {
        static const bool Value_ =
            TIsNullaryCallableCVSelectorImpl<TFunctor, TResult>::Value_
            || TIsNullaryCallableCVSelectorImpl<TFunctor,
                typename TRemoveConst<TResult>::TType_>::Value_
            || TIsNullaryCallableCVSelectorImpl<TFunctor,
                typename TRemoveVolatile<TResult>::TType_>::Value_
            || TIsNullaryCallableCVSelectorImpl<TFunctor,
                typename TRemoveConst<typename TRemoveVolatile<TResult>::TType_
                    >::TType_>::Value_;
    };

    template <class TFunctor>
    struct TIsNullaryCallable<TFunctor, void>
    {
        static const bool Value_ =
            TIsNullaryCallableImpl<TFunctor, void>::Value_;
    };
}

#endif

