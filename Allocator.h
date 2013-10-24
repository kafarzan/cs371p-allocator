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
            // cout << view(1) << endl;

           
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
            // cout << n << endl;
            pointer p;
            // cout << currentSentinel << endl;
            // if(allBytesNeeded == currentSentinel)
            // We need to iterate over the array. Stop when we find a free block.
            while(currentPosition <= N-4 && !freeBlock)
            {
                currentSentinel = view(currentPosition);
                // cout << "current Sentinel at beginning " << currentSentinel << endl;
                // cout << "current position at beginning " << currentPosition << endl;
                // Check if we have enough bytes and if the remaining bytes are enough to allocate another type
                if(currentSentinel >= allBytesNeeded && (currentSentinel - allBytesNeeded) >= smallestBlock)
                {
                    // cout <<  "currentPosition IS1 " << currentPosition << endl;
                    int sentinelVal = -(sizeof(T)*n);
                    // Indices for new sentinels
                    int newFreeFrontSent = currentPosition + allBytesNeeded;
                    int newFreeBackSent = currentPosition + 4 + (view(currentPosition));
                    // cout << view(currentPosition) << " and " << view(currentPosition+view(currentPosition-4))  << endl;

                    //calculate and set freespace sentinels
                    // cout << "Free space left: " << view(currentPosition) - allBytesNeeded << endl;
                    view(newFreeFrontSent) = view(currentPosition)  - allBytesNeeded;
                    view(newFreeBackSent) = view(currentPosition) - allBytesNeeded;
                    // place new sentinels for busy space

                    // Sentinels of what we are trying to allocate get updated to actualBytesNeeded (not all)
                    view(currentPosition) = sentinelVal; 
                    view(currentPosition+(sizeof(T)*n)+4) = sentinelVal;

                    //set the back sentinel of the busy space
                    
                    ///get the pointer of the the sentinel
                   // cout << "Sentinel should be : "<< view(currentPosition) << endl;
  
                    //  cout << "1" << endl;
                   //  cout << "P should be : "<< &view(currentPosition+4) << endl;
                    p = reinterpret_cast<pointer>(&view(currentPosition+4));
                    int* test = reinterpret_cast<int*>(p);

                    // cout << "Sentinel is : " << *(--test) << endl;
                    // cout << n << endl;
                    // cout << "front busy sentinel = "<< view(currentPosition) << " Back busy sentinel = " <<  view(currentPosition+n+4)<< endl;
                    // cout << "front free sentinel = "<< view(newFrontSent) << " Back free sentinel = " <<  view(newBackSent)<< endl;

                    freeBlock = true;
                    assert(freeBlock);
                    assert(valid());
                    return p;
                }
                else if(currentSentinel >= allBytesNeeded-8)
                {

                    // cout << " CURRENT SENTINEl2 " << currentSentinel << endl;

                    // cout <<  "currentPosition IS2 " << currentPosition << endl;

                                        // originalSentinel = view(originalSentinel);
                    int sentinelVal = -currentSentinel;
                    // int newFrontSent = currentPosition +8 + n;
                    // int newBackSent = currentPosition + 8 + n + (view(currentPosition) -8-n);
                    // cout << view(currentPosition) << " and " << view(currentPosition+view(currentPosition-4))  << endl;

                    // view(newFrontSent) = view(currentPosition)  -n-8; // set the freeblock from the current sentinel forward n+8 bytes
                    // view(newBackSent) = view(currentPosition) -n-8; // set the backsentinel to here
                    view(currentPosition) = sentinelVal; // place new sentinels for busy space
                    view(-(currentPosition + view(currentPosition)-4)) = sentinelVal;
                    // cout << "SECOND SENTINEL SHOULD BE AT POSITION : "<<  view(-(currentPosition + view(currentPosition)-4)) << endl;
                    // cout << sentinelVal << endl;

                    
                    //set the back sentinel of the busy space
                    
                    // cout << "2" << endl;
                    // cout << "Sentinel should be : "<< view(currentPosition) << endl;
                    ///get the pointer of the the sentinel
                    p = reinterpret_cast<pointer>(&view(currentPosition+4));
                    
                    int* test = reinterpret_cast<int*>(p);

                    // cout << "Sentinel is : " << *(--test) << endl;

                    // cout << n << endl;
                    // cout << "front busy sentinel = "<< view(currentPosition) << " Back busy sentinel = " <<  view(currentPosition+n+4)<< endl;
                    // cout << "front free sentinel = "<< view(newFrontSent) << " Back free sentinel = " <<  view(newBackSent)<< endl;

                    freeBlock = true;
                    assert(freeBlock);
                    assert(valid());
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
                        // cout << "hello1 " << endl;
                    }
                    else
                    {
                        assert(!freeBlock);
                        assert(currentPosition >= 0);
                        currentPosition += currentSentinel+8;
                        // cout << "hello2 " << endl;
                    }
                }
            }
            // cout << "LOOOK HERERERERE : " << view(96) << endl;;
            assert(!freeBlock);
            // <your code>
            return NULL;}                   // replace!

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
        void deallocate (pointer p, size_type n) {
            
            int* frontSentinelPoint = reinterpret_cast<int*>(p);
            int frontSentinel = *(--frontSentinelPoint);
            frontSentinel = -frontSentinel;

            char* first = reinterpret_cast<char*>(p);
            char *second = first + frontSentinel;
            int firstIndex = first - &a[0] - 4;
            int secondIndex = second - &a[0];
            // cout << "index: " <<  secondIndex << endl;


            view(firstIndex) = frontSentinel;
            view(secondIndex) = frontSentinel;

            if(view(secondIndex + 4) > 0)
            {
                view(firstIndex) = frontSentinel + view(secondIndex + 4) + 8;
                // cout << "FirstSentinel " << view(firstIndex) << endl;
                int nextFirstSentinel = view(secondIndex + 4);
               // cout << "nextFirst Sentinel: " << nextFirstSentinel << endl;
                int nextSecondIndex = secondIndex + 8 + nextFirstSentinel;
                //cout << "nextSecondIndex " << nextSecondIndex << endl;
                view(nextSecondIndex) = nextFirstSentinel + frontSentinel + 8;
                // cout << "next second sentinel " << view(nextSecondIndex) << endl;
               // view(secondIndex + 4 + view(secondIndex + 4)) = 

            }
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
