/*
 * range.hpp                -- range containter
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

#ifndef __RANGE_HPP_2012_01_31__
#define __RANGE_HPP_2012_01_31__

#include <algorithm>
#include <functional>

#include "emptyassert.hpp"
#include "predicates.hpp"
#include "rangeimpl.hpp"

namespace NRaingee
{
    template <class TType, class TAssert = TEmptyAssert>
    class TRange
    {
        IRangeImpl<TType>* Impl_;

        struct TNoCheckTag
        {
        };

        inline TRange(IRangeImpl<TType>* impl, TNoCheckTag)
            : Impl_(impl)
        {
        }

    public:
        typedef typename TSequenceRangeImpl<TType>::TSizeType_ TSizeType_;

        inline explicit TRange(IRangeImpl<TType>* impl = 0)
            : Impl_((impl && !impl->IsEmpty()) ? impl : 0)
        {
        }

        inline TRange(const TRange& range)
            : Impl_(range.IsEmpty() ? 0 : range.Impl_->Clone())
        {
        }

        inline explicit TRange(TSizeType_ size, const TType& value = TType())
            : Impl_(size ? new TSequenceRangeImpl<TType>(size, value) : 0)
        {
        }

        template <class TInputIterator>
        inline TRange(TInputIterator first, TInputIterator last)
            : Impl_(first == last ?
                0 : new TSequenceRangeImpl<TType>(first, last))
        {
        }

        inline TRange& operator =(TRange range)
        {
            Swap(range);
            return *this;
        }

        inline ~TRange()
        {
            delete Impl_;
        }

        IRangeImpl<TType>* Release()
        {
            IRangeImpl<TType>* result = Impl_;
            Impl_ = 0;
            return result;
        }

        inline bool IsEmpty() const
        {
            return !Impl_ || Impl_->IsEmpty();
        }

        inline void Pop()
        {
            TAssert::Assert(Not(BindMember(this, &TRange::IsEmpty)),
                "TRange::Pop called on empty range");
            Impl_->Pop();
        }

        inline const TType& Front() const
        {
            TAssert::Assert(Not(BindMember(this, &TRange::IsEmpty)),
                "TRange::Front called on empty range");
            return Impl_->Front();
        }

        inline void Swap(TRange& range)
        {
            std::swap(Impl_, range.Impl_);
        }

        template <class TCounter>
        inline TRange& operator *=(TCounter counter)
        {
            if (!IsEmpty())
            {
                if (counter)
                {
                    Impl_ = new TRepeatedRangeImpl<TType, TCounter>(Impl_,
                        counter);
                }
                else
                {
                    *this = TRange();
                }
            }
            return *this;
        }

        template <class TCounter>
        friend inline TRange operator *(TRange lhs, TCounter counter)
        {
            lhs *= counter;
            TRange<TType, TAssert> result;
            result.Swap(lhs);
            return result;
        }

        inline TRange& operator +=(TRange range)
        {
            if (IsEmpty())
            {
                Swap(range);
            }
            else if (!range.IsEmpty())
            {
                Impl_ = new TConcatenatedRangesImpl<TType>(Impl_,
                    range.Release());
            }
            return *this;
        }

        friend inline TRange operator +(TRange<TType, TAssert> lhs,
            TRange<TType, TAssert> rhs)
        {
            lhs += TRange(rhs.Release(), TNoCheckTag());
            TRange<TType, TAssert> result;
            result.Swap(lhs);
            return result;
        }

        template <class TCompare>
        inline TRange& Complement(TRange range, TCompare compare)
        {
            if (!IsEmpty() && !range.IsEmpty())
            {
                Impl_ = new TComplementedRangesImpl<TType, TCompare>(Impl_,
                    range.Release(), compare);
            }
            return *this;
        }

        inline TRange& operator -=(TRange range)
        {
            return Complement(TRange(range.Release(), TNoCheckTag()),
                std::less<TType>());
        }

        friend inline TRange operator -(TRange lhs, TRange rhs)
        {
            lhs -= TRange(rhs.Release(), TNoCheckTag());;
            TRange<TType, TAssert> result;
            result.Swap(lhs);
            return result;
        }

        template <class TCompare>
        inline TRange& Unite(TRange range, TCompare compare)
        {
            if (IsEmpty())
            {
                Swap(range);
            }
            else if (!range.IsEmpty())
            {
                Impl_ = new TUnitedRangesImpl<TType, TCompare>(Impl_,
                    range.Release(), compare);
            }
            return *this;
        }

        inline TRange& operator |=(TRange range)
        {
            return Unite(TRange(range.Release(), TNoCheckTag()), std::less<TType>());
        }

        friend inline TRange operator |(TRange lhs, TRange rhs)
        {
            lhs |= TRange(rhs.Release(), TNoCheckTag());
            TRange<TType, TAssert> result;
            result.Swap(lhs);
            return result;
        }

        template <class TCompare>
        inline TRange& Intersect(TRange range, TCompare compare)
        {
            if (IsEmpty() || range.IsEmpty())
            {
                *this = TRange();
            }
            else
            {
                Impl_ = new TIntersectedRangesImpl<TType, TCompare>(Impl_,
                    range.Release(), compare);
            }
            return *this;
        }

        inline TRange& operator &=(TRange range)
        {
            return Intersect(TRange(range.Release(), TNoCheckTag()),
                std::less<TType>());
        }

        friend inline TRange operator &(TRange lhs, TRange rhs)
        {
            lhs &= TRange(rhs.Release(), TNoCheckTag());
            TRange<TType, TAssert> result;
            result.Swap(lhs);
            return result;
        }

        template <class TCompare>
        bool Includes(TRange rhs, TCompare compare) const
        {
            TRange lhs(*this);
            while (!(lhs.IsEmpty() || rhs.IsEmpty()))
            {
                if (compare(lhs.Front(), rhs.Front()))
                {
                    lhs.Pop();
                }
                else if (compare(rhs.Front(), lhs.Front()))
                {
                    return false;
                }
                else
                {
                    lhs.Pop();
                    rhs.Pop();
                }
            }
            return rhs.IsEmpty();
        }

        bool Includes(TRange rhs) const
        {
            return Includes(TRange(rhs.Release(), TNoCheckTag()),
                std::less<TType>());
        }

        friend inline bool operator ==(TRange lhs, TRange rhs)
        {
            while(!(lhs.IsEmpty() || rhs.IsEmpty()))
            {
                if (lhs.Front() != rhs.Front())
                {
                    return false;
                }
                lhs.Pop();
                rhs.Pop();
            }
            return lhs.IsEmpty() && rhs.IsEmpty();
        }

        friend inline bool operator !=(TRange lhs, TRange rhs)
        {
            return !(TRange(lhs.Release(), TNoCheckTag())
                == TRange(rhs.Release(), TNoCheckTag()));
        }
    };
}

namespace std
{
    template <class TType, class TAssert>
    inline void swap(NRaingee::TRange<TType, TAssert>& lhs,
        NRaingee::TRange<TType, TAssert>& rhs)
    {
        lhs.Swap(rhs);
    }
}

#endif

