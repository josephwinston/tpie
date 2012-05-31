// -*- mode: c++; tab-width: 4; indent-tabs-mode: t; c-file-style: "stroustrup"; -*-
// vi:set ts=4 sts=4 sw=4 noet :
// Copyright 2008, 2012, The TPIE development team
// 
// This file is part of TPIE.
// 
// TPIE is free software: you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License as published by the
// Free Software Foundation, either version 3 of the License, or (at your
// option) any later version.
// 
// TPIE is distributed in the hope that it will be useful, but WITHOUT ANY
// WARRANTY; without even the implied warranty of MERCHANTABILITY or
// FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public
// License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with TPIE.  If not, see <http://www.gnu.org/licenses/>

///////////////////////////////////////////////////////////////////////////////
/// \file pq_overflow_heap.h Priority queue overflow heap.
/// \sa \ref priority_queue.h
///////////////////////////////////////////////////////////////////////////////

#ifndef _TPIE_PQ_OVERFLOW_HEAP_H_
#define _TPIE_PQ_OVERFLOW_HEAP_H_

#include <tpie/internal_priority_queue.h>

namespace tpie {

///////////////////////////////////////////////////////////////////////////////
/// \class pq_overflow_heap
/// \author Lars Hvam Petersen
///
/// \brief Overflow Priority Queue, based on a simple Heap.
///////////////////////////////////////////////////////////////////////////////
template<typename T, typename Comparator = std::less<T> >
class pq_overflow_heap {
public:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief Constructor.
    ///
    /// \param maxsize Maximal size of queue.
    ///////////////////////////////////////////////////////////////////////////
    pq_overflow_heap(stream_size_type maxsize, Comparator c=Comparator());

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Insert an element into the priority queue.
    ///
    /// \param x The item.
    ///////////////////////////////////////////////////////////////////////////
    void push(const T& x);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Remove the top element from the priority queue.
    ///////////////////////////////////////////////////////////////////////////
    void pop();

    ///////////////////////////////////////////////////////////////////////////
    /// \brief See what's on the top of the priority queue.
    ///
    /// \return Top element.
    ///////////////////////////////////////////////////////////////////////////
    const T& top();

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Returns the size of the queue.
    ///
    /// \return Queue size.
    ///////////////////////////////////////////////////////////////////////////
    stream_size_type size() const;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Return true if queue is empty otherwise false.
    ///
    /// \return Boolean - empty or not.
    ///////////////////////////////////////////////////////////////////////////
    bool empty() const;

    ///////////////////////////////////////////////////////////////////////////
	/// \brief The factor of the size, total, which is returned sorted.
    ///////////////////////////////////////////////////////////////////////////
    static const double sorted_factor;
		
    ///////////////////////////////////////////////////////////////////////////
    /// \brief Returns whether the overflow heap is full or not.
    ///
    /// \return Boolean - full or not.
    ///////////////////////////////////////////////////////////////////////////
    bool full() const;

    ///////////////////////////////////////////////////////////////////////////
	/// Sorts the underlying array and returns a pointer to it, this operation
	/// invalidates the heap.
    ///
    /// \return A pointer to the sorted underlying array.
    ///////////////////////////////////////////////////////////////////////////
    T* sorted_array();

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Return size of sorted array.
    ///
    /// \return Size.
    ///////////////////////////////////////////////////////////////////////////
    stream_size_type sorted_size() const;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Remove all elements from queue.
    ///////////////////////////////////////////////////////////////////////////
    void sorted_pop();

private:
    Comparator comp;
	internal_priority_queue<T, Comparator> h;
    stream_size_type maxsize;
    //T dummy;
};
	
	template<typename T, typename Comparator>
	const double pq_overflow_heap<T,Comparator>::sorted_factor = 1.0;

#include "pq_overflow_heap.inl"

    namespace ami {
		using tpie::pq_overflow_heap;
    }  //  ami namespace

}  //  tpie namespace

#endif
