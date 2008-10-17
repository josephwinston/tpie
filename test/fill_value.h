// Copyright (c) 1995 Darren Vengroff
//
// File: fill_value.h
// Author: Darren Vengroff <darrenv@eecs.umich.edu>
// Created: 3/6/95
//
// $Id: fill_value.h,v 1.5 2005-11-17 17:07:40 jan Exp $
//
#ifndef _FILL_VALUE_H
#define _FILL_VALUE_H

#include <portability.h>

#include <matrix_fill.h>

template<class T>
class fill_value : public AMI_matrix_filler<T> {
private:
    T value;
public:
    fill_value() : value() {};
    void set_value(const T &v) {
        value = v;
    };
    AMI_err initialize(TPIE_OS_OFFSET /*rows*/, TPIE_OS_OFFSET /*cols*/)
    {
        return AMI_ERROR_NO_ERROR;
    };
    T element(TPIE_OS_OFFSET /*row*/, TPIE_OS_OFFSET /*col*/)
    {
        return value;
    };
};

#endif // _FILL_VALUE_H 