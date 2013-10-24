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

  // --------
  // typedefs
  // --------
template <typename A>
struct TestAllocator : testing::Test {
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

template <typename C>
struct OurAllocator2 : testing::Test {
    typedef          C                  allocator_type;
    typedef typename C::value_type      value_type;
    typedef typename C::difference_type difference_type;
    typedef typename C::pointer         pointer;};

template <typename D>
struct TestValid : testing::Test {
    typedef          D                  allocator_type;
    typedef typename D::value_type      value_type;
    typedef typename D::difference_type difference_type;
    typedef typename D::pointer         pointer;};


typedef testing::Types<
            std::allocator<int>,
            std::allocator<double>,
            Allocator<int, 100>,
            Allocator<double, 100> >
        my_types;
// Two things on each block

typedef testing::Types<
            Allocator<int, 100>,
            Allocator<double, 100>,
            Allocator<int, 300>, 
            Allocator<double, 300>,
            Allocator<long, 300>,
            Allocator<char, 300> >
        our_types;

typedef testing::Types<
            Allocator<int, 128>,
            Allocator<double, 248>,
            Allocator<long, 248>,
            Allocator<char, 38> >
        FillArray;


typedef testing::Types<
            Allocator<int, 120> >
        ValidTests;


TYPED_TEST_CASE(TestAllocator, my_types);

TYPED_TEST(TestAllocator, One) {
    typedef typename TestFixture::allocator_type  allocator_type;
    typedef typename TestFixture::value_type      value_type;
    typedef typename TestFixture::difference_type difference_type;
    typedef typename TestFixture::pointer         pointer;
                    // cout << "break-1" << endl;
    allocator_type x;
    const difference_type s = 1;
    const value_type      v = 2;
    const pointer         p = x.allocate(s);
    if (p != 0) {
        x.construct(p, v);
        ASSERT_EQ(v, *p);
        x.destroy(p);
        x.deallocate(p, s);}
}

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
    int size = -x.view(0) / s;


    int frontSentinel = -(size * s);
    int* realSentinelPoint = reinterpret_cast<int*>(b);
    int realSentinel = *(--realSentinelPoint);
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
    int size = -x.view(0) / s;

    // cout << sizeof(x) << " Total Array bytes" << endl;
    // cout << sizeof(v) << " Type bytes" << endl;
    // cout << s << " types put in" << endl;
    // cout << sizeof(*b) << " pointing to type" << endl;
    
    int frontSentinel = -size * s;
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
    int full = x.view(0);
    pointer         b = x.allocate(s);
    int size = -x.view(0) / s;

    // cout << "  SIZE OF IS " << sizeof(value_type) << endl;
    int pos = 0;
    ASSERT_EQ(-s*size, x.view(pos));
    // Go to next sentinel set
    int spaceAllocated = -x.view(pos);
    pos += s*size + 8;

    // cout << "pos " << pos << endl;
    // Gets sentinel of next free space;
    // cout << "view at pos " << x.view(pos) << endl;
 
    pointer         b2 = x.allocate(s2);
    ASSERT_TRUE(b2);
    pos = 0;
    ASSERT_EQ(-s*size, x.view(pos));
    pos += s*size + 8; 
    ASSERT_EQ(-s2*size, x.view(pos));
    pos += s2*size + 8; 
   ASSERT_EQ(full - s*size - s2*size -16, x.view(pos));

    pointer         b3 = x.allocate(s3);
    ASSERT_TRUE(b3);

    pos = 0;
    ASSERT_EQ(-s*size, x.view(pos));
    pos += s*size + 8; 
    ASSERT_EQ(-s2*size, x.view(pos));
    pos += s2*size + 8; 
    ASSERT_EQ(-s3*size, x.view(pos));
    pos += s3*size + 8; 
   ASSERT_EQ(full - s*size - s2*size - s3*size - 24, x.view(pos));

}

TYPED_TEST(OurAllocator, AllocateTooMuch) {
    typedef typename TestFixture::allocator_type  allocator_type;
    typedef typename TestFixture::value_type      value_type;
    typedef typename TestFixture::difference_type difference_type;
    typedef typename TestFixture::pointer         pointer;

    allocator_type x;
    //Allocate something that will not fit into the allocator
    const difference_type s = 10000000;
    //make sure that the pointer is null
    try{
         pointer         b = x.allocate(s);}
    catch (std::bad_alloc& b){
        cout << "Bad Allocate" << endl;
    }
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
    int freeSpace = x.view(0);
    pointer         b = x.allocate(s);

    //make sure that the pointer is not null
    ASSERT_TRUE(b);


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
    // ASSERT_EQ(x.view(firstIndex), -(sizeof.(v)));
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
    int size = -x.view(0) / s;

    ASSERT_TRUE(b);
    char* x1 = reinterpret_cast<char*>(&(x.view(0)));
    int index = reinterpret_cast<char*>(b)-x1 - 4;
    int secondindex = -x.view(index) + 4 + index;
    ASSERT_EQ(x.view(index), x.view(secondindex));
    //Checking that the first sentinel is allocating only 1
    ASSERT_EQ(x.view(index), -(size));
    
    
    pointer         b2 = x.allocate(s2);
    ASSERT_TRUE(b2);
    x1 = reinterpret_cast<char*>(&(x.view(0)));
    index = reinterpret_cast<char*>(b2)-x1 - 4;
    // We add index to the second index since we might be in the middle of the array
    secondindex = -x.view(index) + 4 + index;
    ASSERT_EQ(x.view(index), x.view(secondindex));
    //Checking that the first sentinel is allocating only 3
    ASSERT_EQ(x.view(index), -(size*s2));


    pointer         b3 = x.allocate(s3);
    ASSERT_TRUE(b3);
    x1 = reinterpret_cast<char*>(&(x.view(0)));
    index = reinterpret_cast<char*>(b3)-x1 - 4;
    secondindex = -x.view(index) + 4 + index;
    ASSERT_EQ(x.view(index), x.view(secondindex));
    //Checking that the first sentinel is allocating only 2
    ASSERT_EQ(x.view(index), -(size*s3));


    //BEGINNING OF DEALOCATION
    x.deallocate(b, s);
    x1 = reinterpret_cast<char*>(&(x.view(0)));
    index = reinterpret_cast<char*>(b)-x1 - 4;
    secondindex = x.view(index) + 4 + index;
    ASSERT_EQ(x.view(index), x.view(secondindex));

    x.deallocate(b2, s2);
    x1 = reinterpret_cast<char*>(&(x.view(0)));
    index = reinterpret_cast<char*>(b2)-x1 - 4;
    secondindex = x.view(index) + 4 + index;
    //ASSERT_EQ(x.view(index), x.view(secondindex));
    ASSERT_EQ(x.view(secondindex), x.view(secondindex - x.view(secondindex) -4));

    x.deallocate(b3, s3);
    x1 = reinterpret_cast<char*>(&(x.view(0)));
    index = reinterpret_cast<char*>(b3)-x1 - 4;
    secondindex = x.view(index) + 4 + index;
    ASSERT_EQ(x.view(secondindex - x.view(secondindex) -4), x.view(4+x.view(secondindex - x.view(secondindex) -4)));

}


TYPED_TEST(OurAllocator, Multiple_Allocations_and_Deallocations) {
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

    int freeSpace = x.view(0);
    //All ALLOCATION IS SAME FROM CHECK ALLOCATE TEST
    pointer         b = x.allocate(s);
    int size = -x.view(0) / s;

    ASSERT_TRUE(b);

    //freeSpace -= (sizeof(v) * s + 8);
    char* x1 = reinterpret_cast<char*>(&(x.view(0)));
    int index = reinterpret_cast<char*>(b)-x1 - 4;
    int secondindex = -x.view(index) + 4;
    // Check sentinels equal
    ASSERT_EQ(x.view(index), x.view(secondindex));
    //Checking sentinel is correct size
    ASSERT_EQ(x.view(index), -(size));
    freeSpace += x.view(index) - 8;
    //Check free space is correct size
    ASSERT_EQ(x.view(secondindex+4), freeSpace);


    pointer         b2 = x.allocate(s2);
    ASSERT_TRUE(b2);
    x1 = reinterpret_cast<char*>(&(x.view(0)));
    index = reinterpret_cast<char*>(b2)-x1 - 4;
    secondindex = -x.view(index) + 4 + index;
    // Check sentinels equal
    ASSERT_EQ(x.view(index), x.view(secondindex));
    // Check sentinels are correct size
    ASSERT_EQ(x.view(index), -(size)*s2);
    freeSpace += x.view(index) - 8;
    // Check free space is correct size
    ASSERT_EQ(x.view(secondindex+4), freeSpace);


    x.deallocate(b, s);
    x1 = reinterpret_cast<char*>(&(x.view(0)));
    index = reinterpret_cast<char*>(b)-x1 - 4;
    // x.view(index) is now positive since it should be deallocated
    secondindex = x.view(index) + 4 + index;
    ASSERT_EQ(x.view(index), x.view(secondindex));



    pointer         b3 = x.allocate(s3);
    ASSERT_TRUE(b3);
    x1 = reinterpret_cast<char*>(&(x.view(0)));
    index = reinterpret_cast<char*>(b3)-x1 - 4;
    secondindex = -x.view(index) + 4 + index;
    // Check sentinels equal
    ASSERT_EQ(x.view(index), x.view(secondindex));
    // Check sentinels are correct size
    ASSERT_EQ(x.view(index), -(size)*s3);
    freeSpace += x.view(index) - 8;
    // Check free space is correct size
    ASSERT_EQ(x.view(secondindex+4), freeSpace);

/*

    x.deallocate(b2, s2);
    x1 = reinterpret_cast<char*>(&(x.view(0)));
    index = reinterpret_cast<char*>(b2)-x1 - 4;
    // x.view(index) is now positive since it should be deallocated
    secondindex = x.view(index) + 4 + index;
    ASSERT_EQ(x.view(index), x.view(secondindex));

    x.deallocate(b3, s3);
    x1 = reinterpret_cast<char*>(&(x.view(0)));
    index = reinterpret_cast<char*>(b)-x1 - 4;
    // x.view(index) is now positive since it should be deallocated
    secondindex = x.view(index) + 4 + index;
    ASSERT_EQ(x.view(index), x.view(secondindex));
    ASSERT_EQ(x.view(secondindex - x.view(secondindex) -4), x.view(4+x.view(secondindex - x.view(secondindex) -4)));



*/
    x.deallocate(b2, s2);

    int * SentinelS = reinterpret_cast<int*>(b2);
    int frontSentinelS = *(--SentinelS);
    // frontSentinelS = -frontSentinelS;

    char * first = reinterpret_cast<char*>(b2);
    char* second = first + frontSentinelS;
    int firstIndex = first - &x.a[0] - 4;
    int secondIndex = second - &x.a[0];
    ASSERT_EQ(x.view(secondIndex), x.view(secondIndex));

    // ASSERT_EQ(x.view(b2))

    x.deallocate(b3, s3);

    SentinelS = reinterpret_cast<int*>(b3);
    frontSentinelS = *(--SentinelS);
    // frontSentinelS = -frontSentinelS;

    first = reinterpret_cast<char*>(b3);
    second = first + frontSentinelS;
    firstIndex = first - &x.a[0] - 4;
    secondIndex = second - &x.a[0];
    ASSERT_EQ(x.view(secondIndex - x.view(secondIndex) -4), x.view(4+x.view(secondIndex - x.view(secondIndex) -4)));

}



TYPED_TEST_CASE(OurAllocator2, FillArray);

TYPED_TEST(OurAllocator2, AllocateEntireArray) {
    typedef typename TestFixture::allocator_type  allocator_type;
    typedef typename TestFixture::value_type      value_type;
    typedef typename TestFixture::difference_type difference_type;
    typedef typename TestFixture::pointer         pointer;

    allocator_type x;
    const difference_type s = 30;
    pointer         b = x.allocate(s);

    ASSERT_TRUE(b);
    char* x1 = reinterpret_cast<char*>(&(x.view(0)));
    int index = reinterpret_cast<char*>(b)-x1 - 4;
    int secondindex = -x.view(index) + 4;
    ASSERT_EQ(x.view(index), x.view(secondindex));
}


TYPED_TEST(OurAllocator2, Allocate_Then_Deallocate_Entire_Array) {
    typedef typename TestFixture::allocator_type  allocator_type;
    typedef typename TestFixture::value_type      value_type;
    typedef typename TestFixture::difference_type difference_type;
    typedef typename TestFixture::pointer         pointer;
    allocator_type x;
    const difference_type s = 30;
    pointer         b = x.allocate(s);

    ASSERT_TRUE(b);
    char* x1 = reinterpret_cast<char*>(&(x.view(0)));
    int index = reinterpret_cast<char*>(b)-x1 - 4;
    int secondindex = -x.view(index) + 4;
    ASSERT_EQ(x.view(index), x.view(secondindex));

    x.deallocate(b, s);

    x1 = reinterpret_cast<char*>(&(x.view(0)));
    index = reinterpret_cast<char*>(b)-x1 - 4;
    secondindex = x.view(index) + 4;
    ASSERT_EQ(x.view(index), x.view(secondindex));

}

TYPED_TEST_CASE(TestValid, ValidTests);


TYPED_TEST(TestValid, Valid1) {
    typedef typename TestFixture::allocator_type  allocator_type;
    typedef typename TestFixture::value_type      value_type;
    typedef typename TestFixture::difference_type difference_type;
    typedef typename TestFixture::pointer         pointer;
    allocator_type x;

    ASSERT_EQ(x.view(0), x.view(x.view(0)+4));

}


// TYPED_TEST(TestValid, Valid5) {
//     typedef typename TestFixture::allocator_type  allocator_type;
//     typedef typename TestFixture::value_type      value_type;
//     typedef typename TestFixture::difference_type difference_type;
//     typedef typename TestFixture::pointer         pointer;
//     allocator_type x;

//     ASSERT_EQ(x.view(0), x.view(x.view(0)+4));

//     x.view(0) = -8;
//     x.view(12) = -8;
//     x.view(16) = -12;
//     x.view(30) = -12;
//     x.view(34) = 76;
//     x.view(116) = 76;

//     cout << &x.a[0] << endl;

//     x.a[0] = -8;
//     x.a[12] = -8;
//     x.a[16] = -12;
//     x.a[30] = -12;
//     x.a[34] = 76;
//     x.a[116] = 76;
//     ASSERT_TRUE(x.valid());

// }
       // const int& = x.view(0)
    //     x.view(0) = -8;
    // &x.a[0] = reinterpret_cast<char*>(&(x.view(0)));
    // x.view(12) = -8;
    // &x.a[12] = reinterpret_cast<char*>(&(x.view(12)));
    // x.view(16) = -8;
    // &x.a[16] = reinterpret_cast<char*>(&(x.view(16)));
    // x.view(30) = -8;
    // &x.a[30] = reinterpret_cast<char*>(&(x.view(30)));
    // x.view(34) = -8;
    // &x.a[34] = reinterpret_cast<char*>(&(x.view(34)));
    // x.view(116) = -8;
    // &x.a[116] = reinterpret_cast<char*>(&(x.view(116)));







    // char* x1 = reinterpret_cast<char*>(&(x.view(0)));
    // int index = reinterpret_cast<char*>(b)-x1;



    // // ASSERT_EQ(x.view(firstIndex), x.view(secondIndex));
    
    // cout << "deallocate2" <<endl;

    // x.deallocate(b2, s2);


    // SentinelS = reinterpret_cast<int*>(b2);
    // frontSentinelS = *(--SentinelS);
    // // frontSentinelS = -frontSentinelS;
    // cout << "FRONT SENTINEL IS : " << frontSentinelS << endl;

    // first = reinterpret_cast<char*>(b2);
    // second = first + frontSentinelS;
    // firstIndex = first - &x.a[0] - 4;
    // secondIndex = second - &x.a[0];

    // cout << "FIRST INDEX IS : " << firstIndex << endl;
    // cout << "Second INDEX IS : " << secondIndex << endl;

    // ASSERT_EQ(x.view(secondIndex), x.view(secondIndex - x.view(secondIndex) -4));

    // // ASSERT_EQ(x.view(b2))

    // x.deallocate(b3, s3);

    // SentinelS = reinterpret_cast<int*>(b3);
    // frontSentinelS = *(--SentinelS);
    //     cout << "FRONT SENTINEL IS : " << frontSentinelS << endl;

    // // frontSentinelS = -frontSentinelS;

    // first = reinterpret_cast<char*>(b3);
    // second = first + frontSentinelS;
    // firstIndex = first - &x.a[0] - 4;
    // secondIndex = second - &x.a[0];

    //     cout << "FIRST INDEX IS : " << firstIndex << endl;
    // cout << "Second INDEX IS : " << secondIndex << endl;
    // cout << "view(secondIndex) " << x.view(secondIndex)<< endl;

    // ASSERT_EQ(x.view(secondIndex), x.view(secondIndex - x.view(secondIndex) -4));