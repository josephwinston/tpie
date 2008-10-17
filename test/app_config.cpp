// Copyright (c) 1994 Darren Erik Vengroff
//
// File: app_config.cpp
// Author: Darren Erik Vengroff <darrenv@eecs.umich.edu>
// Created: 10/7/94
//




#include "app_config.h"

#ifdef NDEBUG
bool verbose = false;
#else
bool verbose = true;
#endif

TPIE_OS_SIZE_T test_mm_size = DEFAULT_TEST_MM_SIZE;
TPIE_OS_OFFSET test_size = DEFAULT_TEST_SIZE;
int random_seed = 17;
