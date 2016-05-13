// Copyright (c) 2009-2016 The Regents of the University of Michigan
// This file is part of the HOOMD-blue project, released under the BSD 3-Clause License.


// this include is necessary to get MPI included before anything else to support intel MPI
#include "hoomd/ExecutionConfiguration.h"

#include <iostream>

#include <math.h>
#include "hoomd/ClockSource.h"
#include "hoomd/Profiler.h"
#include "hoomd/Variant.h"

//! Name the unit test module
#define BOOST_TEST_MODULE UtilityClassesTests
#include "boost_utf_configure.h"

/*! \file utils_test.cc
    \brief Unit tests for ClockSource, Profiler, and Variant
    \ingroup unit_tests
*/

using namespace std;

// the clock test depends on timing and thus should not be run in automatic builds.
// uncomment to test by hand if the test seems to be behaving poorly

//! perform some simple checks on the clock source code
/*BOOST_AUTO_TEST_CASE(ClockSource_test)
    {
    ClockSource c1;
    int64_t t = c1.getTime();
    // c.getTime() should read 0, but we can't expect it to be exact, so allow a tolerance
    BOOST_CHECK(abs(int(t)) <= 1000000);

    // test timing a whole second
    ClockSource c2;
    int64_t t1 = c2.getTime();
    Sleep(1000);
    int64_t t2 = c2.getTime();

    BOOST_CHECK(abs(int(t2 - t1 - int64_t(1000000000))) <= 20000000);*/

// unfortunately, testing of microsecond timing with a sleep routine is out of the question
// the following test code tests the ability of the timer to read nearby values
/*ClockSource c4;
int64_t times[100];
for (int i = 0; i < 100; i++)
    {
    times[i] = c4.getTime();
    }
for (int i = 0; i < 100; i++)
    {
    cout << times[i] << endl;
    }*/

// test copying timers
// operator=
/*  c1 = c2;
    t1 = c1.getTime();
    t2 = c2.getTime();
    BOOST_CHECK(abs(int(t1-t2)) <= 1000000);
    // copy constructor
    ClockSource c3(c1);
    t1 = c1.getTime();
    t2 = c3.getTime();
    BOOST_CHECK(abs(int(t1-t2)) <= 1000000);

    // test the ability of the clock source to format values
    BOOST_CHECK_EQUAL(ClockSource::formatHMS(0), string("00:00:00"));
    BOOST_CHECK_EQUAL(ClockSource::formatHMS(int64_t(1000000000)), string("00:00:01"));
    BOOST_CHECK_EQUAL(ClockSource::formatHMS(int64_t(1000000000)*int64_t(11)), string("00:00:11"));
    BOOST_CHECK_EQUAL(ClockSource::formatHMS(int64_t(1000000000)*int64_t(65)), string("00:01:05"));
    BOOST_CHECK_EQUAL(ClockSource::formatHMS(int64_t(1000000000)*int64_t(3678)), string("01:01:18"));
    }*/

//! perform some simple checks on the profiler code
BOOST_AUTO_TEST_CASE(Profiler_test)
    {
    // ProfileDataElem tests
    // constructor test
    ProfileDataElem p;
    BOOST_CHECK(p.getChildElapsedTime() == 0);
    BOOST_CHECK(p.getTotalFlopCount() == 0);
    BOOST_CHECK(p.getTotalMemByteCount() == 0);

    // build up a tree and test its getTotal members
    p.m_elapsed_time = 1;
    p.m_flop_count = 2;
    p.m_mem_byte_count = 3;
    BOOST_CHECK(p.getChildElapsedTime() == 0);
    BOOST_CHECK(p.getTotalFlopCount() == 2);
    BOOST_CHECK(p.getTotalMemByteCount() == 3);

    p.m_children["A"].m_elapsed_time = 4;
    p.m_children["A"].m_flop_count = 5;
    p.m_children["A"].m_mem_byte_count = 6;
    BOOST_CHECK(p.getChildElapsedTime() == 4);
    BOOST_CHECK(p.getTotalFlopCount() == 7);
    BOOST_CHECK(p.getTotalMemByteCount() == 9);

    p.m_children["B"].m_elapsed_time = 7;
    p.m_children["B"].m_flop_count = 8;
    p.m_children["B"].m_mem_byte_count = 9;
    BOOST_CHECK(p.getChildElapsedTime() == 4+7);
    BOOST_CHECK(p.getTotalFlopCount() == 7+8);
    BOOST_CHECK(p.getTotalMemByteCount() == 9+9);

    p.m_children["A"].m_children["C"].m_elapsed_time = 10;
    p.m_children["A"].m_children["C"].m_flop_count = 11;
    p.m_children["A"].m_children["C"].m_mem_byte_count = 12;
    BOOST_CHECK(p.getChildElapsedTime() == 4+7);
    BOOST_CHECK(p.getTotalFlopCount() == 7+8+11);
    BOOST_CHECK(p.getTotalMemByteCount() == 9+9+12);

    Profiler prof("Main");
    prof.push("Loading");
    Sleep(500);
    prof.pop();
    prof.push("Neighbor");
    Sleep(1000);
    prof.pop(int64_t(1e6), int64_t(1e6));

    prof.push("Pair");
    prof.push("Load");
    Sleep(1000);
    prof.pop(int64_t(1e9), int64_t(1e9));
    prof.push("Work");
    Sleep(1000);
    prof.pop(int64_t(10e9), int64_t(100));
    prof.push("Unload");
    Sleep(1000);
    prof.pop(int64_t(100), int64_t(1e9));
    prof.pop();

    std::cout << prof;

    // This code attempts to reproduce the problem found in ticket #50
    Profiler prof2("test");
    prof2.push("test1");
    //Changing this slep value much lower than 100 results in the bug.
    Sleep(000);
    prof2.pop(100, 100);
    std::cout << prof2;

    }

//! perform some simple checks on the variant types
BOOST_AUTO_TEST_CASE(Variant_test)
    {
    Variant v;
    BOOST_CHECK_EQUAL(v.getValue(0), 0.0);
    BOOST_CHECK_EQUAL(v.getValue(100000), 0.0);
    v.setOffset(1000);
    BOOST_CHECK_EQUAL(v.getValue(0), 0.0);
    BOOST_CHECK_EQUAL(v.getValue(100000), 0.0);
    }

//! perform some simple checks on the variant types
BOOST_AUTO_TEST_CASE(VariantConst_test)
    {
    double val = 10.5;
    VariantConst v(val);
    BOOST_CHECK_EQUAL(v.getValue(0), val);
    BOOST_CHECK_EQUAL(v.getValue(100000), val);
    v.setOffset(1000);
    BOOST_CHECK_EQUAL(v.getValue(0), val);
    BOOST_CHECK_EQUAL(v.getValue(100000), val);
    }

//! perform some simple checks on the variant types
BOOST_AUTO_TEST_CASE(VariantLinear_test1)
    {
    double val = 10.5;
    VariantLinear v;
    v.setPoint(500, val);
    BOOST_CHECK_EQUAL(v.getValue(0), val);
    BOOST_CHECK_EQUAL(v.getValue(500), val);
    BOOST_CHECK_EQUAL(v.getValue(100000), val);
    v.setOffset(1000);
    BOOST_CHECK_EQUAL(v.getValue(0), val);
    BOOST_CHECK_EQUAL(v.getValue(500), val);
    BOOST_CHECK_EQUAL(v.getValue(100000), val);
    }

//! perform some simple checks on the variant types
BOOST_AUTO_TEST_CASE(VariantLinear_test2)
    {
    VariantLinear v;
    v.setPoint(500, 10.0);
    v.setPoint(1000, 20.0);

    BOOST_CHECK_CLOSE(v.getValue(0), 10.0, tol);
    BOOST_CHECK_CLOSE(v.getValue(500), 10.0, tol);
    BOOST_CHECK_CLOSE(v.getValue(750), 15.0, tol);
    BOOST_CHECK_CLOSE(v.getValue(1000), 20.0, tol);
    BOOST_CHECK_CLOSE(v.getValue(1500), 20.0, tol);
    v.setOffset(1000);
    BOOST_CHECK_CLOSE(v.getValue(0), 10.0, tol);
    BOOST_CHECK_CLOSE(v.getValue(1000), 10.0, tol);
    BOOST_CHECK_CLOSE(v.getValue(1500), 10.0, tol);
    BOOST_CHECK_CLOSE(v.getValue(1750), 15.0, tol);
    BOOST_CHECK_CLOSE(v.getValue(2000), 20.0, tol);
    BOOST_CHECK_CLOSE(v.getValue(2500), 20.0, tol);
    }

//! perform some simple checks on the variant types
BOOST_AUTO_TEST_CASE(VariantLinear_test3)
    {
    VariantLinear v;
    v.setPoint(500, 10.0);
    v.setPoint(1000, 20.0);
    v.setPoint(2000, 50.0);

    BOOST_CHECK_CLOSE(v.getValue(0), 10.0, tol);
    BOOST_CHECK_CLOSE(v.getValue(500), 10.0, tol);
    BOOST_CHECK_CLOSE(v.getValue(750), 15.0, tol);
    BOOST_CHECK_CLOSE(v.getValue(1000), 20.0, tol);
    BOOST_CHECK_CLOSE(v.getValue(1500), 35.0, tol);
    BOOST_CHECK_CLOSE(v.getValue(2000), 50.0, tol);
    BOOST_CHECK_CLOSE(v.getValue(2500), 50.0, tol);
    v.setOffset(1000);
    BOOST_CHECK_CLOSE(v.getValue(0), 10.0, tol);
    BOOST_CHECK_CLOSE(v.getValue(1000), 10.0, tol);
    BOOST_CHECK_CLOSE(v.getValue(1500), 10.0, tol);
    BOOST_CHECK_CLOSE(v.getValue(1750), 15.0, tol);
    BOOST_CHECK_CLOSE(v.getValue(2000), 20.0, tol);
    BOOST_CHECK_CLOSE(v.getValue(2500), 35.0, tol);
    BOOST_CHECK_CLOSE(v.getValue(3000), 50.0, tol);
    BOOST_CHECK_CLOSE(v.getValue(3500), 50.0, tol);

    // mix up the order to make sure it works no matter what
    BOOST_CHECK_CLOSE(v.getValue(3000), 50.0, tol);
    BOOST_CHECK_CLOSE(v.getValue(1500), 10.0, tol);
    BOOST_CHECK_CLOSE(v.getValue(0), 10.0, tol);
    BOOST_CHECK_CLOSE(v.getValue(2000), 20.0, tol);
    BOOST_CHECK_CLOSE(v.getValue(2500), 35.0, tol);
    BOOST_CHECK_CLOSE(v.getValue(1000), 10.0, tol);
    BOOST_CHECK_CLOSE(v.getValue(1750), 15.0, tol);
    BOOST_CHECK_CLOSE(v.getValue(3500), 50.0, tol);
    }
