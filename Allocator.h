// ------------------------------
// projects/allocator/Allocator.h
// Copyright (C) 2013
// Glenn P. Downing
// ------------------------------

#ifndef Allocator_h
#define Allocator_h

// --------
// includes
// --------

#include <cassert> // assert
#include <cstddef> // ptrdiff_t, size_t
#include <new>     // new

// ---------
// Allocator
// ---------
using namespace std;

template <typename T, int N>
class Allocator {
    public:
        // --------
        // typedefs
        // --------

        typedef T                 value_type;

        typedef std::size_t       size_type;
        typedef std::ptrdiff_t    difference_type;

        typedef       value_type*       pointer;
        typedef const value_type* const_pointer;

        typedef       value_type&       reference;
        typedef const value_type& const_reference;

    public:
        // -----------
        // operator ==
        // -----------

        friend bool operator == (const Allocator&, const Allocator&) {
            return true;}                                              // this is correct

        // -----------
        // operator !=
        // -----------

        friend bool operator != (const Allocator& lhs, const Allocator& rhs) {
            return !(lhs == rhs);}

    private:
        // ----
        // data
        // ----

        char a[N];

        // -----
        // valid
        // -----

        /**
         * O(1) in space
         * O(n) in time
         * <your documentation>
         */
        bool valid () const {
            cout << "****************************************************************************************" << endl;
            int i = 0;
            while(i < N)
            {
                cout << view(i) << " and ";
                // cout << view(i) << " and " << view(view(i) + 4) << endl;
                bool negative = false;
                int temp = view(i);
                // cout << "TEMP IS : " << temp << endl;
                if( temp < 0)
                {
                    // cout << "enter";
                    negative = true;
                    temp = -temp;
                    // cout << temp << endl;
                } 
                // cout  << view(i+temp+4) << " " ;
                int temp2 = view(i+temp+4);
                cout << view(i+temp+4) << " ";
                // cout << "TEMP2 IS : " << temp2 << endl;
                // cout << "VIEW I + 4 " << view(i+temp + 8)<<endl;

                if(temp2 < 0)
                {
                    temp2 = -temp2;
                }
                if( temp != temp2)
                {
                     // cout << "break 1" << endl;
                    return false;
                }
                else if(!negative && view(i+temp+8) > 0 && i+temp+8 != N)
                {

                    // cout << "next value is " << view(i+temp+8) <<  " break 2" << endl;
                    return false;
                }
                i+=temp+8;
                // cout << "check i "<<  i << endl; 
            }
            cout << endl;
            cout << "end of valid" << endl;

            return true;}
        /**
         * O(1) in space
         * O(1) in time
         * <your documentation>
         */
        int& view (int i) {
            return *reinterpret_cast<int*>(&a[i]);}

    public:
        // ------------
        // constructors
        // ------------

        /**
         * O(1) in space
         * O(1) in time
         * <your documentation>
         */
         //default constructor
        Allocator () {
             // <your code>
            //what if less than 8
            int freeSpace = N-8;

            int frontSentinel = freeSpace;
            int backSentinel = freeSpace;

            //place the fornt and back sentinel of freespace into the array
            view(0) = frontSentinel;
            view(N-4) = backSentinel;

           
            assert(valid());}
        // Default, copy, destructor, and copy assignment
        // Allocator  (const Allocator&);
        // ~Allocator ();
        // Allocator& operator = (const Allocator&);

        // --------
        // allocate
        // --------

        /**
         * O(1) in space
         * O(n) in time
         * <your documentation>
         * after allocation there must be enough space left for a valid block
         * the smallest allowable block is sizeof(T) + (2 * sizeof(int))
         * choose the first block that fits
         */
        pointer allocate (size_type n) {
            assert(sizeof(T)*n > 0);

            int smallestBlock = sizeof(T) + (2 * sizeof(int));
            // int originalSentinel;
            //current position in the array
            int currentPosition = 0;
            int currentSentinel = view(currentPosition); // sentinel at current position
            bool freeBlock = false;
            int frontSentinel = sizeof(T)*n;
            int backSentinel = sizeof(T)*n;
            int allBytesNeeded = sizeof(T)*n + 8;
            pointer p;

            // We need to iterate over the array. Stop when we find a free block.
            while(currentPosition < N-4 && !freeBlock)
            {
                currentSentinel = view(currentPosition);
                // Check if we have enough bytes and if the remaining bytes are enough to allocate another type
                if(currentSentinel >= allBytesNeeded && (currentSentinel - allBytesNeeded) >= smallestBlock)
                {
                    int sentinelVal = -(sizeof(T)*n);
                    // cout << " Setting this sentinel to: " << sentinelVal << endl;
                    // Indices for new sentinels
                    int newFreeFrontSent = currentPosition + allBytesNeeded;
                    int newFreeBackSent = currentPosition + 4 + (view(currentPosition));

                    //calculate and set freespace sentinels
                    // cout << "currentPosition " << currentPosition << endl;
                    // cout << view(currentPosition) - allBytesNeeded << endl;
                    view(newFreeFrontSent) = view(currentPosition)  - allBytesNeeded;
                    // cout << " newFreeBackSent " << newFreeBackSent << endl;
                    view(newFreeBackSent) = view(newFreeFrontSent);
                    // place new sentinels for busy space

                    // Sentinels of what we are trying to allocate get updated to actualBytesNeeded (not all)
                    // cout << " current position " << currentPosition << " and next position " << currentPosition+(sizeof(T)*n)+4<< endl;
                    view(currentPosition) = sentinelVal; 
                    view(currentPosition+(sizeof(T)*n)+4) = sentinelVal;
                    
                    ///get the pointer of the the sentinel
                    p = reinterpret_cast<pointer>(&view(currentPosition+4));
                    // int* test = reinterpret_cast<int*>(p);

                    freeBlock = true;
                    assert(freeBlock);
                    assert(valid());
                    return p;
                }
                else if(currentSentinel >= allBytesNeeded)
                {

                    int sentinelVal = -currentSentinel;
                    // cout << " Setting this sentinel to: " << sentinelVal << endl;
                    view(currentPosition) = sentinelVal; // place new sentinels for busy space
                    // cout << "LOOOOK" << -(currentPosition + view(currentPosition)-4) << endl;
                    view(-(currentPosition + view(currentPosition)-4)) = sentinelVal;


                    ///get the pointer of the the sentinel
                    p = reinterpret_cast<pointer>(&view(currentPosition+4));

                    freeBlock = true;
                    assert(freeBlock);
                    assert(valid());
                    return p;

                }
                else if(currentSentinel == allBytesNeeded -8)
                {
                    view(0) = -(view(0));
                    view(allBytesNeeded-4) = -view(allBytesNeeded-4);
                    p = reinterpret_cast<pointer>(&view(currentPosition+4));
                    return p;
                }
                else
                {
                    if(currentSentinel < 0)
                    {
                        assert(!freeBlock);
                        int temp = -currentSentinel;
                        assert (temp >= 0);
                        currentPosition += temp+8;
                    }
                    else
                    {
                        assert(!freeBlock);
                        assert(currentPosition >= 0);
                        currentPosition += currentSentinel+8;
                    }
                }
            }
            assert(!freeBlock);
            return NULL;}           

        // ---------
        // construct
        // ---------

        /**
         * O(1) in space
         * O(1) in time
         * <your documentation>
         */
        void construct (pointer p, const_reference v) {
            new (p) T(v);                               // this is correct and exempt
            assert(valid());}                           // from the prohibition of new

        // ----------
        // deallocate
        // ----------

        /**
         * O(1) in space
         * O(1) in time
         * <your documentation>
         * after deallocation adjacent free blocks must be coalesced
         */
        void deallocate (pointer p, size_type) {
            if(!p)
                return;
            
            // cout << "************************************************************************************* " << endl;
            int* frontSentinelPoint = reinterpret_cast<int*>(p);
            int frontSentinel = *(--frontSentinelPoint);
            frontSentinel = -frontSentinel;

            char* first = reinterpret_cast<char*>(p);
            char *second = first + frontSentinel;
            int firstIndex = first - &a[0] - 4;
            int secondIndex = second - &a[0];

           // cout << "firstIndex " << firstIndex << endl;
            // cout  << "FRONT sentinel " << frontSentinel << endl;
            view(firstIndex) = frontSentinel;
            view(secondIndex) = frontSentinel;
           // cout << "second sentinel:  " << secondIndex<< endl;
            // bool found = false;
            // cout << " first index : " << firstIndex << endl;
            // cout << " view first index - 4 : " << view(firstIndex - 4) << endl;

            if(firstIndex > 0 && view(firstIndex - 4) >= 0)
            {
                // cout << "ENTERD1 " << endl;
                // cout << "original first index is " << firstIndex << endl;
                // cout << "new first index is " << firstIndex - 8 -view(firstIndex-4) << endl;

                firstIndex = firstIndex - view(firstIndex-4)-8;
                // cout << " firstIndex is now " << firstIndex << endl;
                view(firstIndex) = view(firstIndex) + view(secondIndex) + 8;
                view(secondIndex) = view(firstIndex);
            }
            // cout << "HELLO" << endl;
            // cout << " second index : " << secondIndex << endl;
            // cout << " view second index + 4 : " << view(secondIndex + 4) << endl;
            // cout <<  " next index is :  **************"<< view(secondIndex+4) << endl;
            if( secondIndex < N-4 && view(secondIndex + 4) > 0)
            {
                // cout << "FIRST INDEX value IS NOW " << view(firstIndex) << endl;
                // cout << "ENTERD2 " << endl;

                // cout << " LINE 1 " << endl;
                view(firstIndex) = view(firstIndex) + view(secondIndex + 4) + 8;
                // cout << " LINE 2 " << endl;
                int nextFirstSentinel = view(secondIndex + 4);
                // cout << " LINE 3 " << endl;
                int nextSecondIndex = secondIndex + 8 + nextFirstSentinel;
                // cout << " LINE 4 " << endl;
                // cout << " nextFirstSentinel + frontSentinel + 8 = " << nextFirstSentinel + frontSentinel + 8<< endl;
                view(nextSecondIndex) = view(firstIndex);
                // found = true;
            }
            // cout << " found " << found << endl;

            assert(valid());}

        // -------
        // destroy
        // -------

        /**
         * O(1) in space
         * O(1) in time
         * <your documentation>
         */
        void destroy (pointer p) {
            p->~T();               // this is correct
            assert(valid());}

        /**
         * O(1) in space
         * O(1) in time
         * <your documentation>
         */
        const int& view (int i) const {
            return *reinterpret_cast<const int*>(&a[i]);}};

#endif // Allocator_h
