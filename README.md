# Signal
A simple signal system that's meant to be a type safe implementation of the observer pattern that does not require a specific observer class and is simple to use, this requires C++17

A simple example in with a Main function
```cpp
// SignalTests.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include "Signal.h"

void BasicFunction()
{
    std::cout << "a function taking no arguments" << std::endl;
}
void AddNumbers(int a, int b)
{
    std::cout << (a + b) << std::endl;
}

class Foo
{
public:
    void Print()
    {
        std::cout << "Foo print from " << this << std::endl;
    }
};

void Fighter(Foo *aFoo)
{
    std::cout << aFoo << " is a real fighter" << std::endl;
}

void PrintSquared(float aNum)
{
    std::cout << (aNum * aNum) << std::endl;
}

int main()
{
    Signal basicSignal;

    // connecting a function without an object
    basicSignal.Connect<&BasicFunction>();
    // this does not work, it's type safe so it will not compile and complain about wrong argument types and counts
    //basicSignal.Connect<&AddNumbers>();

    // connect an object with a member function
    Foo foo;
    basicSignal.Connect<&Foo::Print>(&foo);

    // connect an object with a non-member function
    basicSignal.Connect<&Fighter>(&foo);

    // Emits the signal, calls all connected functions with the objects in the order they were connected
    basicSignal.Emit();

    // Disconnecting has the same syntax
    basicSignal.Disconnect<&BasicFunction>();
    basicSignal.Disconnect<&Foo::Print>(&foo);
    basicSignal.Disconnect<&Fighter>(&foo);

    // Signals can be made to take arguments as well
    Signal<float> advancedSignal;

    // the function still needs to take the proper arguments
    advancedSignal.Connect<&PrintSquared>();

    // Emitting needs the proper arguments for the signal
    advancedSignal.Emit(3.2);
}

// a function taking no arguments
// Foo print from 0000003D94B5F9A4
// 0000003D94B5F9A4 is a real fighter
// 10.24
```

Creating a signal
```cpp
Signal mySignal; // this one takes no arguments
Signal<int, float> myOtherSignal; // this one takes an 'int' and a 'float'
```
Connecting to it
```cpp
// with a global function
void SomeFunction();
mySignal.Connect<&SomeFunction>();

// with a member function
class SomeClass {
public:
  void SomeClassFunction();
};
SomeClass myObject;
mySignal.Connect<&SomeClass::SomeClassFunction>(&myObject);

// with an object pointer connected as first argument to a function
void SomeOtherFunction(SomeClass *aObject);
mySignal.Connect<&SomeOtherFunction>(&myObject);
```
Emitting
```cpp
mySignal.Emit();
myOtherSignal.Emit(42, 3.22f);
```
Disconnecting, this has the same syntax as connecting
```cpp
mySignal.Disconnect<&SomeFunction>();
mySignal.Disconnect<&SomeClass::SomeClassFunction>(&myObject);
mySignal.Disconnect<&SomeOtherFunction>(&myObject);
```
