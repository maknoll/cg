/************************************************************************/
/* This program benchmarks different ways to iterate over arrays and    */
/* contains examples on how to work with variables, pointers and        */
/* references.                                                          */
/* Note: All conceptually multi-dimensional arrays, such as a matrix,   */
/* should be stored in memory as a one-dimensional array!               */
/* Avoid multi-dimensional arrays in C++!                               */
/************************************************************************/

//Include for in/out streams (std::cout/cin/cerr)
#include <iostream>

//Include for STL vector container
#include <vector>

//Include Timer
#include "Timer.hpp"

//Identify whether the program is compiled in debug-mode or not
#ifdef _DEBUG
#define SIZE 3000 //choose a smaller array size due to slower execution
#else
#define SIZE 6000
#endif

//Iterate in two loops over all elements in the array
//The memory is not processed in a sequential order
//This is highly inefficient!
inline void experiment1(double* field, int x, int y)
{
    for(int i = 0; i < x;++i)
    {
        for(int j = 0; j < y;++j)
        {
            field[i+j*x]=1.0;
        }
    }
}

//The memory is processed sequentially by two loops
inline void experiment2(double* field, int x, int y)
{
    for(int j = 0; j < y;++j)
    {
        for(int i = 0; i < x;++i)
        {
            field[i+j*x]=1.0;
        }
    }
}

//The memory is processed sequentially by a single loop
inline void experiment3(double* field, int x, int y)
{
    int size = x*y;

    for(int i = 0; i < size;++i)
    {
        field[i]=1.0;
    }
}

//The memory is processed backwards sequentially by a single loop
inline void experiment4(double* field, int x, int y)
{
    int size = x*y;

    for(int i = size-1; i;--i)
    {
        field[i]=1.0;
    }
}

//The memory is processed backwards sequentially by a single loop
inline void experiment5(double* field, int x, int y)
{
    int size = x*y/4;

    for(int i = 0; i < size; ++i)
    {
        field[4*i]=1.0;
        field[4*i+1]=1.0;
        field[4*i+2]=1.0;
        field[4*i+3]=1.0;
    }
}

void doExperiments(int size)
{
    std::cout<<"Starting memory experiments of size "<< size <<std::endl;

    //Allocate memory for the experiments
    double* field = new double[size*size];

    util::StopWatch timer;

    std::cout<<"Experiment1 ";
    timer.tic();
    experiment1(field,size,size);
    timer.toc(std::cout);

    std::cout<<"Experiment2 ";
    timer.tic();
    experiment2(field,size,size);
    timer.toc(std::cout);

    std::cout<<"Experiment3 ";
    timer.tic();
    experiment3(field,size,size);
    timer.toc(std::cout);

    std::cout<<"Experiment4 ";
    timer.tic();
    experiment4(field,size,size);
    timer.toc(std::cout);

    std::cout<<"Experiment5 ";
    timer.tic();
    experiment5(field,size,size);
    timer.toc(std::cout);

    //Don't forget to delete the experiment's array memory!
    delete[] field;
    std::cout<<"Finished..."<<std::endl;
}

void memoryFun()
{
    //Primitives on stack
    {
        double value0 = 0.0;
        double value1(0.0);

        value0 += value1; //WAR warning
    }//value0 and value1 are removed from the stack according to LIFO principle

    //Primitives on heap
    {
        //Create a double on the heap
        double* value0 = new double(0.0);

        //Don't forget to free the memory
        delete value0;

        //The pointer value0 still points to the freed memory, operating on value0 would crash the program:
        //*value0 = 10;
        //by convention you should set the pointer to zero
        value0 = 0;
    }

    //References
    {
        double value0 = 0.0;

        //Get a reference to the value
        double& refValue0 = value0;
        refValue0 = 1.0; //this changes value0 to 1.0
    }

    //Static Arrays
    {
        //A static array on the stack, the length 4 must be known at compile time!
        int a[4] = {0,1,2,3};
        a[0]= 10; //Use the array-operator [] to assign values to array entries
        a[1]= 20;
        a[2]= 30;
        a[3]= 40;
        a[0]++;
        //a = {10,20,30,40}

    }//the static array a is now destroyed, i.e. removed from the stack -> LIFO

    //Dynamic Arrays
    {
        //Create a dynamic array of size 4 on the heap.
        //In contrast to JAVA, arrays do not have a length() function
        //By inspecting "a", it is impossible to determine its length is 4
        //So remember to keep track of your dynamic array sizes
        int* a = new int[4];
        //a points to the memory address of the first entry in the array

        //The array-operator [] can be used just like on static arrays to access all entries
        a[0]=0;
        a[1]=1;
        a[2]=2;
        a[3]=3; //Set the entries such that a ={0,1,2,3}

        *a = 10; //Sets the value of the first element to 10 by dereferencing the pointer

        *(a+1) = 20; //Do pointer arithmetic and dereferencing to set the second entry to 20

        int* aPtr = a+2; //create a copy of the pointer "a" and add two, thus aPtr points to the third element

        *(aPtr++) = 30; //assign 30 to the third value by dereferencing
                        //and you postfix increment, so that aPtr points to the fourth element afterwards

        *aPtr = 40; //dereference and assign value 40 to the fourth element

        //a = {10,20,30,40}
        std::cout<<"Array={"<<a[0]<<","<<a[1]<<","<<a[2]<<","<<a[3]<<"}"<<std::endl;

        delete [] a; //don't forget to free the allocated memory
        //if you didn't call delete[], the memory for a would still remain allocated until the program ends
        //this is in contrast to the JAVA Garbage Collector that frees elements that are not being referenced anywhere
    }

    //STL container (recommended)
    {
      std::vector<int> a; //create a vector that has *int* as element type

      a.reserve(4); //this is not required by reserving memory can speed up
                    //vector filling

      a.push_back(0);
      a.push_back(1);
      a.push_back(2);
      a.push_back(3);
      //push values into vector, a.size() is 4 now. Note that the vector
      //grows automatically to account for required memory amount).

      a[2] = 10; //use array indexing

      std::cout<<"Array={"<<a[0]<<","<<a[1]<<","<<a[2]<<","<<a[3]<<"}"<<std::endl;

      //The memory of a is free *automatically* when a gets destroyed!!
      //No need to keep track of memory like this
    }
}

int main(int argc, char** argv)
{
    memoryFun();

    //This function creates an array of dimensions SIZE*SIZE
    //and times different ways to iterate over all elements
    doExperiments(SIZE);

#ifdef WIN32
#ifndef __MINGW32__ || __MINGW64__
    int wait;
    std::cin>>wait; //actually lame
#endif
#endif

    return 0;
}
