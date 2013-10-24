// ------------------------------------
// projects/allocator/TestAllocator.c++
// Copyright (C) 2013
// Glenn P. Downing
// ------------------------------------

/*
To test the program:
    % ls -al /usr/include/gtest/
    ...
    gtest.h
    ...

    % locate libgtest.a
    /usr/lib/libgtest.a

    % locate libpthread.a
    /usr/lib/x86_64-linux-gnu/libpthread.a
    /usr/lib32/libpthread.a

    % locate libgtest_main.a
    /usr/lib/libgtest_main.a

    % g++ -pedantic -std=c++0x -Wall TestAllocator.c++ -o TestAllocator -lgtest -lpthread -lgtest_main

    % valgrind TestAllocator > TestAllocator.out
*/

// --------
// includes
// --------

#include <algorithm> // count
#include <memory>    // allocator
#include <iostream>
#include "gtest/gtest.h"
#define private public
#define protected public
#define class struct
#include "Allocator.h"

// -------------
// TestAllocator
// -------------
using namespace std;
template <typename A>
struct TestAllocator : testing::Test {
    // --------
    // typedefs
    // --------

    typedef          A                  allocator_type;
    typedef typename A::value_type      value_type;
    typedef typename A::difference_type difference_type;
    typedef typename A::pointer         pointer;};



template <typename B>
struct OurAllocator : testing::Test {
    typedef          B                  allocator_type;
    typedef typename B::value_type      value_type;
    typedef typename B::difference_type difference_type;
    typedef typename B::pointer         pointer;};


typedef testing::Types<
            std::allocator<int>,
            std::allocator<double>,
            Allocator<int, 100>,
            Allocator<double, 100> >
        my_types;

typedef testing::Types<
            Allocator<int, 100>,
            Allocator<double, 100>,
            Allocator<int, 300>, 
            Allocator<double, 300>,
            Allocator<long, 300>,
            Allocator<char, 300> >
        our_types;


TYPED_TEST_CASE(TestAllocator, my_types);

TYPED_TEST(TestAllocator, One) {
    typedef typename TestFixture::allocator_type  allocator_type;
    typedef typename TestFixture::value_type      value_type;
    typedef typename TestFixture::difference_type difference_type;
    typedef typename TestFixture::pointer         pointer;

    allocator_type x;
    const difference_type s = 1;
    const value_type      v = 2;
    const pointer         p = x.allocate(s);
    if (p != 0) {
        x.construct(p, v);
        ASSERT_EQ(v, *p);
        x.destroy(p);
        x.deallocate(p, s);}}

TYPED_TEST(TestAllocator, Ten) {
    typedef typename TestFixture::allocator_type  allocator_type;
    typedef typename TestFixture::value_type      value_type;
    typedef typename TestFixture::difference_type difference_type;
    typedef typename TestFixture::pointer         pointer;

    allocator_type x;
    const difference_type s = 10;
    const value_type      v = 2;
    const pointer         b = x.allocate(s);
    if (b != 0) {
        pointer e = b + s;
        pointer p = b;
        try {
            while (p != e) {
                x.construct(p, v);
                ++p;}}
        catch (...) {
            while (b != p) {
                --p;
                x.destroy(p);}
            x.deallocate(b, s);
            throw;}
        ASSERT_EQ(s, std::count(b, e, v));
        while (b != e) {
            --e;
            x.destroy(e);}
        x.deallocate(b, s);}}




TYPED_TEST_CASE(OurAllocator, our_types);



TYPED_TEST(OurAllocator, Sentinels) {
    typedef typename TestFixture::allocator_type  allocator_type;
    typedef typename TestFixture::value_type      value_type;
    typedef typename TestFixture::difference_type difference_type;
    typedef typename TestFixture::pointer         pointer;

    allocator_type x;
    const difference_type s = 2;
    const value_type      v = 2;
    pointer         b = x.allocate(s);


    // cout << sizeof(x) << " Total Array bytes" << endl;
    // cout << sizeof(v) << " Type bytes" << endl;
    // cout << s << " types put in" << endl;

    int frontSentinel = -sizeof(v) * s;
    int* realSentinelPoint = reinterpret_cast<int*>(b);
    int realSentinel = *(--realSentinelPoint);
    //cout << "front: " << frontSentinel << " real " << realSentinel << endl;
    ASSERT_EQ(realSentinel, frontSentinel);
    if (b != 0) {
        pointer e = b + s;
        pointer p = b;
        try {
            while (p != e) {
                x.construct(p, v);
                ++p;}}
        catch (...) {
            while (b != p) {
                --p;
                x.destroy(p);}
            x.deallocate(b, s);
            throw;}
        }
    }


TYPED_TEST(OurAllocator, OneDoubleSentinel) {
    typedef typename TestFixture::allocator_type  allocator_type;
    typedef typename TestFixture::value_type      value_type;
    typedef typename TestFixture::difference_type difference_type;
    typedef typename TestFixture::pointer         pointer;

    allocator_type x;
    const difference_type s = 1;
    const value_type      v = 2;
    pointer         b = x.allocate(s);



    // cout << sizeof(x) << " Total Array bytes" << endl;
    // cout << sizeof(v) << " Type bytes" << endl;
    // cout << s << " types put in" << endl;
    // cout << sizeof(*b) << " pointing to type" << endl;
    
    int frontSentinel = -sizeof(v) * s;
    int* realSentinelPoint = reinterpret_cast<int*>(b);
    int realSentinel = *(--realSentinelPoint);
    
    //cout << "front: " << frontSentinel << " real " << realSentinel << endl;
    ASSERT_EQ(realSentinel, frontSentinel);
    if (b != 0) {
        pointer e = b + s;
        pointer p = b;
        try {
            while (p != e) {
                x.construct(p, v);
                ++p;}}
        catch (...) {
            while (b != p) {
                --p;
                x.destroy(p);}
            x.deallocate(b, s);
            throw;}
    }
 }

// TYPED_TEST(OurAllocator, CheckDefaultConstructors) {
//     typedef typename TestFixture::allocator_type  allocator_type;
//     typedef typename TestFixture::value_type      value_type;
//     typedef typename TestFixture::difference_type difference_type;
//     typedef typename TestFixture::pointer         pointer;

//     allocator_type x;
//     // int frontSentinel = -sizeof(v) * s;
//     // int* realSentinelPoint = reinterpret_cast<int*>(b);
//     // int realSentinel = *(--realSentinelPoint);
//     const int allocatorSize = x.view(0);

//     const difference_type s = 1;
//     const value_type      v = 2;
//     // pointer         b = x.allocate(s);

//     ASSERT_EQ(x.view(0), sizeof(x)-8);
//     ASSERT_EQ(x.view(sizeof(x)-4), sizeof(x)-8);
// }

//Checks that 1 T is allocated, then 2T's are allocated to the free space that is left
//then 3T's are allocated to the free space that is left
TYPED_TEST(OurAllocator, CheckAllocate) {
    typedef typename TestFixture::allocator_type  allocator_type;
    typedef typename TestFixture::value_type      value_type;
    typedef typename TestFixture::difference_type difference_type;
    typedef typename TestFixture::pointer         pointer;

    allocator_type x;
    const difference_type s = 1;
    const value_type      v = 1;
    const difference_type s2 = 3;
    const value_type      v2 = 3;
    const difference_type s3 = 2;
    const value_type      v3 = 2;
    pointer         b = x.allocate(s);
    //make sure that the pointer is not null
    ASSERT_TRUE(b);

    int freeSpace = sizeof(x)-8;
    //freeSpace -= (sizeof(v) * s + 8);

    int* SentinelS = reinterpret_cast<int*>(b);
    int frontSentinelS = *(--SentinelS);
    frontSentinelS = -frontSentinelS;
    // cout << frontSentinelS << endl;

    char* first = reinterpret_cast<char*>(b);
    char *second = first + frontSentinelS;
    int firstIndex = first - &x.a[0] - 4;
    int secondIndex = second - &x.a[0];
    
    //Checking that the first sentinel is allocating the currect amount
    ASSERT_EQ(x.view(firstIndex), -(sizeof(v)));
    freeSpace += x.view(firstIndex) - 8;
    //Checking that the first sentinel of the adjacent free space is of correct size
    ASSERT_EQ(x.view(secondIndex+4), freeSpace);

    pointer         b2 = x.allocate(s2);
    ASSERT_TRUE(b2);


    SentinelS = reinterpret_cast<int*>(b2);
    frontSentinelS = *(--SentinelS);
    frontSentinelS = -frontSentinelS;

    first = reinterpret_cast<char*>(b2);
    second = first + frontSentinelS;
    firstIndex = first - &x.a[0] - 4;
    secondIndex = second - &x.a[0];

    ASSERT_EQ(x.view(firstIndex), -(sizeof(v2))*s2);
    freeSpace += x.view(firstIndex) - 8;
    ASSERT_EQ(x.view(secondIndex+4), freeSpace);

    pointer         b3 = x.allocate(s3);
    ASSERT_TRUE(b3);

    SentinelS = reinterpret_cast<int*>(b3);
    frontSentinelS = *(--SentinelS);
    frontSentinelS = -frontSentinelS;

    first = reinterpret_cast<char*>(b3);
    second = first + frontSentinelS;
    firstIndex = first - &x.a[0] - 4;
    secondIndex = second - &x.a[0];

    ASSERT_EQ(x.view(firstIndex), -(sizeof(v3))*s3);
    freeSpace += x.view(firstIndex) - 8;
    ASSERT_EQ(x.view(secondIndex+4),freeSpace);
}

TYPED_TEST(OurAllocator, AllocateTooMuch) {
    typedef typename TestFixture::allocator_type  allocator_type;
    typedef typename TestFixture::value_type      value_type;
    typedef typename TestFixture::difference_type difference_type;
    typedef typename TestFixture::pointer         pointer;

    allocator_type x;
    //Allocate something that will not fit into the allocator
    const difference_type s = 10000000;
    const value_type      v = 1;

    pointer         b = x.allocate(s);
    //make sure that the pointer is null
    ASSERT_FALSE(b);
}


TYPED_TEST(OurAllocator, CheckDeallocate_1) {
    typedef typename TestFixture::allocator_type  allocator_type;
    typedef typename TestFixture::value_type      value_type;
    typedef typename TestFixture::difference_type difference_type;
    typedef typename TestFixture::pointer         pointer;

    //ALLOCATE 1T into the allocator
    allocator_type x;
    const difference_type s = 1;
    const value_type      v = 1;
    pointer         b = x.allocate(s);
    //make sure that the pointer is not null
    ASSERT_TRUE(b);

    int freeSpace = sizeof(x)-8;
    //freeSpace -= (sizeof(v) * s + 8);

    int* SentinelS = reinterpret_cast<int*>(b);
    int frontSentinelS = *(--SentinelS);
    frontSentinelS = -frontSentinelS;
    // cout << frontSentinelS << endl;

    char* first = reinterpret_cast<char*>(b);
    char *second = first + frontSentinelS;
    int firstIndex = first - &x.a[0] - 4;
    int secondIndex = second - &x.a[0];
    
    //Checking that the first sentinel is allocating the currect amount
    ASSERT_EQ(x.view(firstIndex), -(sizeof(v)));
    freeSpace += x.view(firstIndex) - 8;
    //Checking that the first sentinel of the adjacent free space is of correct size
    ASSERT_EQ(x.view(secondIndex+4), freeSpace);
    freeSpace = x.view(0);
    //Begin Deallocating
    x.deallocate(b,s);
    // cout << x.view(0) + 4 << endl;
    ASSERT_EQ(x.view(0),x.view(x.view(0)+4));

}

TYPED_TEST(OurAllocator, CheckDeallocate_2) {
    typedef typename TestFixture::allocator_type  allocator_type;
    typedef typename TestFixture::value_type      value_type;
    typedef typename TestFixture::difference_type difference_type;
    typedef typename TestFixture::pointer         pointer;

    allocator_type x;
    const difference_type s = 1;
    const value_type      v = 1;
    const difference_type s2 = 3;
    const value_type      v2 = 3;
    const difference_type s3 = 2;
    const value_type      v3 = 2;

    //All ALLOCATION IS SAME FROM CHECK ALLOCATE TEST
    pointer         b = x.allocate(s);
    //make sure that the pointer is not null
    ASSERT_TRUE(b);

    int freeSpace = sizeof(x)-8;
    //freeSpace -= (sizeof(v) * s + 8);

    int* SentinelS = reinterpret_cast<int*>(b);
    int frontSentinelS = *(--SentinelS);
    frontSentinelS = -frontSentinelS;
    // cout << frontSentinelS << endl;

    char* first = reinterpret_cast<char*>(b);
    char *second = first + frontSentinelS;
    int firstIndex = first - &x.a[0] - 4;
    int secondIndex = second - &x.a[0];
    
    //Checking that the first sentinel is allocating the currect amount
    ASSERT_EQ(x.view(firstIndex), -(sizeof(v)));
    freeSpace += x.view(firstIndex) - 8;
    //Checking that the first sentinel of the adjacent free space is of correct size
    ASSERT_EQ(x.view(secondIndex+4), freeSpace);

    pointer         b2 = x.allocate(s2);
    ASSERT_TRUE(b2);


    SentinelS = reinterpret_cast<int*>(b2);
    frontSentinelS = *(--SentinelS);
    frontSentinelS = -frontSentinelS;

    first = reinterpret_cast<char*>(b2);
    second = first + frontSentinelS;
    firstIndex = first - &x.a[0] - 4;
    secondIndex = second - &x.a[0];

    ASSERT_EQ(x.view(firstIndex), -(sizeof(v2))*s2);
    freeSpace += x.view(firstIndex) - 8;
    ASSERT_EQ(x.view(secondIndex+4), freeSpace);

    pointer         b3 = x.allocate(s3);
    ASSERT_TRUE(b3);

    SentinelS = reinterpret_cast<int*>(b3);
    frontSentinelS = *(--SentinelS);
    frontSentinelS = -frontSentinelS;

    first = reinterpret_cast<char*>(b3);
    second = first + frontSentinelS;
    firstIndex = first - &x.a[0] - 4;
    secondIndex = second - &x.a[0];

    ASSERT_EQ(x.view(firstIndex), -(sizeof(v3))*s3);
    freeSpace += x.view(firstIndex) - 8;
    ASSERT_EQ(x.view(secondIndex+4),freeSpace);

    //BEGINNING OF DEALOCATION


    x.deallocate(b, s);

    SentinelS = reinterpret_cast<int*>(b);
    frontSentinelS = *(--SentinelS);
    // frontSentinelS = -frontSentinelS;
    // cout << frontSentinelS << endl;

    first = reinterpret_cast<char*>(b);
    second = first + frontSentinelS;
    firstIndex = first - &x.a[0] - 4;
    secondIndex = second - &x.a[0];

    ASSERT_EQ(x.view(firstIndex), x.view(secondIndex));

    x.deallocate(b2, s2);

    SentinelS = reinterpret_cast<int*>(b2);
    frontSentinelS = *(--SentinelS);
    // frontSentinelS = -frontSentinelS;

    first = reinterpret_cast<char*>(b2);
    second = first + frontSentinelS;
    firstIndex = first - &x.a[0] - 4;
    secondIndex = second - &x.a[0];

    ASSERT_EQ(x.view(firstIndex), x.view(secondIndex));

    // ASSERT_EQ(x.view(b2))

    x.deallocate(b3, s3);

    SentinelS = reinterpret_cast<int*>(b3);
    frontSentinelS = *(--SentinelS);
    // frontSentinelS = -frontSentinelS;

    first = reinterpret_cast<char*>(b3);
    second = first + frontSentinelS;
    firstIndex = first - &x.a[0] - 4;
    secondIndex = second - &x.a[0];

    ASSERT_EQ(x.view(firstIndex), x.view(secondIndex));

}

