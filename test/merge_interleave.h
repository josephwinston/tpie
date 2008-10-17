// Copyright (c) 1994 Darren Erik Vengroff
//
// File: merge_interleave.h
// Author: Darren Erik Vengroff <darrenv@eecs.umich.edu>
// Created: 10/6/94
//
// $Id: merge_interleave.h,v 1.5 2004-02-05 17:42:07 jan Exp $
//
// A merge object to interleave two streams.
//
#ifndef _MERGE_INTERLEAVE_H
#define _MERGE_INTERLEAVE_H

namespace tpie {

    namespace ami {
	
	template<class T> class merge_interleave /*: AMI_merge_base<T> */{
	private:
	    T hold;
	    
	    // States are :
	    //  1 - hold was read from in[0].
	    //  2 - hold was read from in[1].
	    // -1 - hold was read from in[0] and in[1] is empty.
	    // -2 - hold was read from in[1] and in[0] is empty.
	    //  0 - both in[0] and in[1] are empty.
	    int state;
	    
	public:
	    unsigned long int called;
	    
	    err initialize(arity_t arity, CONST T * CONST *in,
			   merge_flag *taken_flags,
			   int &taken_index);
	    
	    err operate(CONST T * CONST *in, merge_flag *taken_flags,
			int &taken_index, T *out);
	    
	    err main_mem_operate(T* mm_stream, TPIE_OS_SIZE_T len);
	    TPIE_OS_SIZE_T space_usage_overhead(void);
	    TPIE_OS_SIZE_T space_usage_per_stream(void);    
	};
	
	template<class T>
	err merge_interleave<T>::initialize(arity_t arity, CONST T * CONST *in,
					    merge_flag * /*taken_flags*/,
					    int &taken_index) {
	    called = 0;
	    
	    if (arity != 2) {
		return OBJECT_INITIALIZATION;
	    }
	    
	    if (in[0] != NULL) {
		hold = *in[0];
		taken_index = 0;
		state = 1;
	    } else if (in[1] != NULL) {
		hold = *in[0];
		taken_index = 0;
		state = -2;
	    } else {
		taken_index = -1;
		state = 0;
		return MERGE_DONE;
	    }
	    
	    return NO_ERROR;
	};
	
		
	template<class T>
	err merge_interleave<T>::operate(CONST T * CONST *in,
					 merge_flag * /*taken_flags*/,
					 int &taken_index,
					 T *out) {
	    called++;
	    
	    // This should actually be changed to interleave any number of
	    // input streams, and use a mod operator on the state to determine
	    // next state and which in[] to take from.
	    
	    switch (state) {
	    case 1:
		*out = hold;
		if (in[1] == NULL) {
		    if (in[0] == NULL) {                
			state = 0;
			taken_index = -1;
		    } else {
			hold = *in[0];
			taken_index = 0;
			state = -1;
		    }
		} else {
		    hold = *in[1];
		    taken_index = 1;
		    state = 2;
		}
		
		return MERGE_OUTPUT;
		
	    case 2:
		*out = hold;
		if (in[0] == NULL) {
		    if (in[1] == NULL) {                
			state = 0;
			taken_index = -1;
		    } else {
			hold = *in[1];
			taken_index = 1;
			state = -2;
		    }
		} else {
		    hold = *in[0];
		    taken_index = 0;
		    state = 1;
		}
		
		return MERGE_OUTPUT;

	    case -1:
		*out = hold;
		if (in[0] == NULL) {                
		    state = 0;
		    taken_index = -1;
		} else {
		    hold = *in[0];
		    taken_index = 0;
		    state = -1;
		}
		return MERGE_OUTPUT;

	    case -2:
		*out = hold;
		if (in[1] == NULL) {                
		    state = 0;
		    taken_index = -1;
		} else {
		    hold = *in[1];
		    taken_index = 1;
		    state = -2;
		}
		return MERGE_OUTPUT;
		
	    case 0:
		return MERGE_DONE;

	    }
	    // Just to keep the compiler happy, since it does not like a
	    // non-void function to end without returning.
	    tp_assert(0, "Control should never reach this point.");

	    return MERGE_DONE;
	};

	template<class T>
	err merge_interleave<T>::main_mem_operate(T* /*mm_stream*/,
						  TPIE_OS_SIZE_T /*len*/) {
	    return NO_MAIN_MEMORY_OPERATION;
	};
	
	template<class T>
	TPIE_OS_SIZE_T merge_interleave<T>::space_usage_overhead(void) {
	    return sizeof(*this);
	}
	
	template<class T>
	TPIE_OS_SIZE_T merge_interleave<T>::space_usage_per_stream(void) {
	    return 0;
	}

    }  //  ami namespace

}  //  tpie namespace

#endif // _MERGE_INTERLEAVE_H 










