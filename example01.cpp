/*
 *  Created on: Apr 10, 2013
 *      Author: martinez
 */

#include <iostream>                                     // for standard output streams
#include <memory>                                       // for smart pointer (e.g., std::shared_ptr)
#include <string>                                       // for string objects (std::string)

// abstract base class
class Animal {
public:
  Animal() {                                            // class constructor
    std::cout << "Animal created " << std::endl;
  }

  virtual ~Animal() {                                   // destructor (required to be **virtual** because we have abstract methods)
    std::cout << "Animal destoyed " << std::endl;
  }

  virtual void move(const std::string &dest) = 0;       // **pure** virtual / abstract method
};

// derived class
class Fish : public Animal {
public:
  Fish() : Animal() {                                   // class constructor
    std::cout << "Fish created " << std::endl;
  }

  void move(const std::string &dest) override {         // implementation of virtual method
    std::cout << "Fish: swim to " << dest << std::endl;
  }
};

// derived class
class Bird : public Animal {
public:
  Bird() : Animal() {                                   // class constructor
    std::cout << "Bird created " << std::endl;
  }

  void move(const std::string &dest) override {         // implementation of virtual method
    std::cout << "Bird: fly to " << dest << std::endl;
  }

  void move(int x, int y) {                             // a non-virtual method
    std::cout << "Bird: fly to " <<
      x << " " << y << std::endl;
  }
};

int main() {

  {
    //object on **stack** with *scope* lifetime
    Fish shark;
    shark.move("Nemo");
  }


  {
    Bird* bird_ptr = nullptr;
    {
      //object on **stack** *without* scope lifetime (constructed with new operator)
      bird_ptr = new Bird();

      bird_ptr->move("Tree");

      //reference to the object on the heap
      Bird& bird = *bird_ptr;
      bird.move("Tree again");
      bird.move(100, 10);
    }

    //we need to **explicitly** delete / clean up memory and objects allocated on heap!!
    delete bird_ptr; bird_ptr = nullptr;
  }

  {
    {
      //object on heap managed by smart pointer with *scope* lifetime
      std::shared_ptr<Fish> fish_sptr = std::make_shared<Fish>();

      fish_sptr->move("See");
    }
    //the object got deleted because all smart pointers referencing it got destroyed,
    //no need to to anything else (this is good for, e.g., robust code and ownership transfer)
  }
}
