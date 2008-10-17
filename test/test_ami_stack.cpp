// Copyright (c) 1994 Darren Vengroff
//
// File: test_ami_stack.cpp
// Author: Darren Vengroff <darrenv@eecs.umich.edu>
// Created: 12/15/94
//

#include <tpie/portability.h>

#include <tpie/progress_indicator_arrow.h>

#include "app_config.h"        
#include "parse_args.h"

// Get the AMI_stack definition.
#include <tpie/stack.h>

using namespace tpie;

struct options app_opts[] = {
    { 0, NULL, NULL, NULL, 0 }
};

void parse_app_opts(int idx, char *opt_arg) {
}

int main(int argc, char **argv)
{
    progress_indicator_arrow* pi = new progress_indicator_arrow("Title","Desc",0,100,1);

    parse_args(argc, argv, app_opts, parse_app_opts);

    if (verbose) {
	std::cout << "test_size = " << test_size << "." << std::endl;
	std::cout << "test_mm_size = " << static_cast<TPIE_OS_OUTPUT_SIZE_T>(test_mm_size) << "." << std::endl;
	std::cout << "random_seed = " << random_seed << "." << std::endl;
    } else {
	std::cout << test_size << ' ' << static_cast<TPIE_OS_OUTPUT_SIZE_T>(test_mm_size) << ' ' << random_seed;
    }
    
    // Set the amount of main memory:
    MM_manager.set_memory_limit(test_mm_size);

    ami::stack<TPIE_OS_OFFSET> stack;

    pi->set_percentage_range(0,test_size);
    pi->set_description("Push");

    // Push values.
    TPIE_OS_OFFSET ii;
    for (ii = test_size; ii--; ) {
	pi->step_percentage();
	stack.push(ii);
    }
    pi->done("Done");

    if (verbose) {
	std::cout << "Stack size = " << stack.size() << std::endl;
    }
    
    // Pop them all off.
    TPIE_OS_OFFSET *jj;
    TPIE_OS_OFFSET last;
    TPIE_OS_OFFSET read = 0;
    stack.pop(&jj);
    last = *jj;
    
    pi->set_description("Pop ");
    pi->reset();
    read++;
    pi->step_percentage();
    while(!stack.is_empty()) {
	ami::err ae = stack.pop(&jj);
	if(ae != ami::NO_ERROR) {
	    std::cout << "Error from stack received" << std::endl;
	}
	read++;
	pi->step_percentage();
	if(*jj != ++last) {
	    std::cout << std::endl << "Error in output: " << *jj << "!=" << last  << std::endl;
	}
    }
    pi->done("Done");
    if(read != test_size) {
	std::cout << "Error: Wrong amount of elements read, got: " << read << " expected: "<<test_size << std::endl;
    }

    if (verbose) {
        std::cout << "Stack size = " << stack.size() << std::endl;
    }
    
    return 0;
}