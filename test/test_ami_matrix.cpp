// Copyright (c) 1994 Darren Vengroff
//
// File: test_ami_matrix.cpp
// Author: Darren Vengroff <darrenv@eecs.umich.edu>
// Created: 12/11/94
//

#include <portability.h>




#include "app_config.h"        
#include "parse_args.h"

// Get AMI_scan().
#include <scan.h>

// Utitlities for ascii output.
#include <scan_utils.h>

// Get matrices.
#include <matrix.h>
#include <matrix_fill.h>
#include "fill_upper_tri.h"

#include <cpu_timer.h>

static char def_crf[] = "osc.txt";
static char def_irf[] = "osi.txt";
static char def_frf[] = "osf.txt";

static char *count_results_filename = def_crf;
static char *intermediate_results_filename = def_irf;
static char *final_results_filename = def_frf;

static bool report_results_count = false;
static bool report_results_intermediate = false;
static bool report_results_final = false;

struct options app_opts[] = {
  { 10, "count-results-filename", "", "C", 1 },
  { 11, "report-results-count", "", "c", 0 },
  { 12, "intermediate-results-filename", "", "I", 1 },
  { 13, "report-results-intermediate", "", "i", 0 },
  { 14, "final-results-filename", "", "F", 1 },
  { 15, "report-results-final", "", "f", 0 },
  { 0, NULL, NULL, NULL, 0 }
};

void parse_app_opts(int idx, char *opt_arg)
{
    switch (idx) {
        case 10:
            count_results_filename = opt_arg;
        case 11:
            report_results_count = true;
            break;
        case 12:
            intermediate_results_filename = opt_arg;
        case 13:
            report_results_intermediate = true;
            break;
        case 14:
            final_results_filename = opt_arg;
        case 15:
            report_results_final = true;
            break;
    }
}


int main(int argc, char **argv)
{
    AMI_err ae;

	test_size = 128 * 1024;

    parse_args(argc, argv, app_opts, parse_app_opts);

    if (verbose) {
      std::cout << "test_size = " << test_size << "." << std::endl;
      std::cout << "test_mm_size = " << static_cast<TPIE_OS_OUTPUT_SIZE_T>(test_mm_size) << "." << std::endl;
      std::cout << "random_seed = " << random_seed << "." << std::endl;
    } else {
        std::cout << test_size << ' ' << static_cast<TPIE_OS_OUTPUT_SIZE_T>(test_mm_size) << ' ';
    }
    
    // Set the amount of main memory:
    MM_manager.set_memory_limit (test_mm_size);

    TPIE_OS_OFFSET matrix_size = static_cast<TPIE_OS_OFFSET>(sqrt(static_cast<double>(test_size)));

    AMI_matrix<double> em0(matrix_size, matrix_size);
    AMI_matrix<double> em1(matrix_size, matrix_size);
    AMI_matrix<double> em2(matrix_size, matrix_size);
        
    // Streams for reporting values to ascii streams.
    
    std::ofstream *osc;
    std::ofstream *osi;
    std::ofstream *osf;
    cxx_ostream_scan<double> *rptc = NULL;
    cxx_ostream_scan<double> *rpti = NULL;
    cxx_ostream_scan<double> *rptf = NULL;
    
    if (report_results_count) {
        osc = new std::ofstream(count_results_filename);
        rptc = new cxx_ostream_scan<double>(osc);
    }
    
    if (report_results_intermediate) {
        osi = new std::ofstream(intermediate_results_filename);
        rpti = new cxx_ostream_scan<double>(osi);
    }
    
    if (report_results_final) {
        osf = new std::ofstream(final_results_filename);
        rptf = new cxx_ostream_scan<double>(osf);
    }

#if 1    
    // Write some doubles.

    fill_upper_tri<double> fut(3.1415927);
    
    {
        ae = AMI_matrix_fill(&em0, &fut);

        if (verbose) {
	  std::cout << "Wrote the initial sequence of values." << std::endl;
        }
        
        if (report_results_count) {
            ae = AMI_scan(&em0, rptc);
            std::cout << "Stream length = " << em0.stream_len() << std::endl;
        }
    }

    {
        ae = AMI_matrix_fill(&em1, &fut);

        if (verbose) {
	  std::cout << "Wrote the second sequence of values." << std::endl;
            std::cout << "Stream length = " << em1.stream_len() << std::endl;
        }
        
        if (report_results_intermediate) {
            ae = AMI_scan(&em1, rpti);
        }
    }
#endif

    cpu_timer cput;
    
    cput.reset();
    cput.start();
    
    // Multiply the two

    ae = AMI_matrix_mult(em0, em1, em2);

    cput.stop();

    std::cout << cput << '\n';
    
    if (verbose) {
      std::cout << "Multiplied them." << std::endl;
        std::cout << "Stream length = " << em2.stream_len() << std::endl;
    }
    
    if (report_results_final) {
        ae = AMI_scan(&em2, rptf);
    }
    
    return 0;
}
