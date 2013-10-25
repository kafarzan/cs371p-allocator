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
         * \return if the allocator is valid
         */
        bool valid () const {
            int totalSpace = 0;
            int totalNodes = 0;
            int i = 0;
            while(i < N)
            {
                bool negative = false;
                int temp = view(i);
                //if negative check using positives
                if( temp < 0)
                {
                    negative = true;
                    temp = -temp;
                } 
                totalSpace+=temp;
                int temp2 = view(i+temp+4);

                //if negative check it using positives
                if(temp2 < 0)
                {
                    temp2 = -temp2;
                }
                if( temp != temp2)
                {
                    return false;
                }

                //if positive and either adjacent block is positive, it is invalid
                else if(!negative && view(i+temp+8) > 0 && i+temp+8 != N)
                {
                    return false;
                }
                i+=temp+8;
                totalNodes += 2;
            }
            if(totalSpace + (totalNodes*4) != N)
            {
                return false;
            }

            return true;}
        /**
         * O(1) in space
         * O(1) in time
         * \param takes in an int
         * \return returns the address in the allocator 
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
         * \fn Allocator
         * \brief This is the default contstructor
         */
         //default constructor
        Allocator () {
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
         * \param how many types you are putting in the allocator
         * \return pointer to the beginning of the allocator
         * \brief if a bad pointer returns null
         * after allocation there must be enough space left for a valid block
         * the smallest allowable block is sizeof(T) + (2 * sizeof(int))
         * choose the first block that fits
         */
        pointer allocate (size_type n) {
            assert(sizeof(T)*n > 0);

            int smallestBlock = sizeof(T) + (2 * sizeof(int));
            //current position in the array
            int currentPosition = 0;
            int currentSentinel = view(currentPosition); // sentinel at current position
            bool freeBlock = false;
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
                    // Indices for new sentinels
                    int newFreeFrontSent = currentPosition + allBytesNeeded;
                    int newFreeBackSent = currentPosition + 4 + (view(currentPosition));

                    //calculate and set freespace sentinels
                    view(newFreeFrontSent) = view(currentPosition)  - allBytesNeeded;
                    view(newFreeBackSent) = view(newFreeFrontSent);
                    // place new sentinels for busy space

                    // Sentinels of what we are trying to allocate get updated to actualBytesNeeded (not all)
                    view(currentPosition) = sentinelVal; 
                    view(currentPosition+(sizeof(T)*n)+4) = sentinelVal;
                    
                    ///get the pointer of the the sentinel
                    p = reinterpret_cast<pointer>(&view(currentPosition+4));

                    freeBlock = true;
                    assert(freeBlock);
                    assert(valid());
                    return p;
                }
                //if not enough room for 1 type and 2 sentinels give entire block
                else if(currentSentinel >= allBytesNeeded)
                {

                    int sentinelVal = -currentSentinel;
                    view(currentPosition) = sentinelVal; // place new sentinels for busy space
                    view(-(currentPosition + view(currentPosition)-4)) = sentinelVal;


                    ///get the pointer of the the sentinel
                    p = reinterpret_cast<pointer>(&view(currentPosition+4));

                    freeBlock = true;
                    assert(freeBlock);
                    assert(valid());
                    return p;

                }
                //if exact size give the entire block
                else if(currentSentinel == allBytesNeeded -8)
                {
                    view(0) = -(view(0));
                    view(allBytesNeeded-4) = -view(allBytesNeeded-4);
                    p = reinterpret_cast<pointer>(&view(currentPosition+4));
                    return p;
                }
                //move to the next block and see if free
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
            // throw std::bad_alloc p;
            return NULL;}           

        // ---------
        // construct
        // ---------

        /**
         * O(1) in space
         * O(1) in time
         * \param p is the pointer to the beginning of the block
         * \param v is what you are putting in that space
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
         * \param p is the pointer to the beginning of the block
         * \details size_type is the type in the block of memory
         * after deallocation adjacent free blocks must be coalesced
         */
        void deallocate (pointer p, size_type) {
            if(!p)
                return;
            
            int* frontSentinelPoint = reinterpret_cast<int*>(p);
            int frontSentinel = *(--frontSentinelPoint);
            frontSentinel = -frontSentinel;

            char* first = reinterpret_cast<char*>(p);
            char *second = first + frontSentinel;
            int firstIndex = first - &a[0] - 4;
            int secondIndex = second - &a[0];

            view(firstIndex) = frontSentinel;
            view(secondIndex) = frontSentinel;

            if(firstIndex > 0 && view(firstIndex - 4) >= 0)
            {

                firstIndex = firstIndex - view(firstIndex-4)-8;
                view(firstIndex) = view(firstIndex) + view(secondIndex) + 8;
                view(secondIndex) = view(firstIndex);
            }
            if( secondIndex < N-4 && view(secondIndex + 4) > 0)
            {
                view(firstIndex) = view(firstIndex) + view(secondIndex + 4) + 8;
                int nextFirstSentinel = view(secondIndex + 4);
                int nextSecondIndex = secondIndex + 8 + nextFirstSentinel;
                view(nextSecondIndex) = view(firstIndex);
                // found = true;
            }

            assert(valid());}

        // -------
        // destroy
        // -------

        /**
         * O(1) in space
         * O(1) in time
         * \param p is the pointer to the block of memory to free
         */
        void destroy (pointer p) {
            p->~T();               // this is correct
            assert(valid());}

        /**
         * O(1) in space
         * O(1) in time
         * \param i is the index to look at
         * \return the address of that index
         */
        const int& view (int i) const {
            return *reinterpret_cast<const int*>(&a[i]);}};

#endif // Allocator_h
