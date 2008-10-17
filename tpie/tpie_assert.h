// Copyright (c) 1994 Darren Erik Vengroff
//
// File: tpie_assert.h
// Author: Darren Erik Vengroff <dev@cs.duke.edu>
// Created: 5/12/94
//
// $Id: tpie_assert.h,v 1.11 2005-11-08 17:21:02 adanner Exp $
//

#ifndef _TPIE_ASSERT_H
#define _TPIE_ASSERT_H

// Get definitions for working with Unix and Windows
#include <tpie/portability.h>
#include <iostream>
#include <tpie/tpie_log.h>

#if DEBUG_ASSERTIONS

#define tp_assert(condition,message) { \
  if (!(condition)) { \
   TP_LOG_FATAL_ID("Assertion failed:"); \
   TP_LOG_FATAL_ID(message); \
    std::cerr << "Assertion failed: " << message << "\n"; \
    assert(condition); \
  } \
}

#else
#define tp_assert(condition,message)
#endif

#endif // _TPIE_ASSERT_H
