#include "CustomQueue.h"
#include <iostream>
#include <string>
#include <cstdlib>

int main()
{
    CustomQueue<int> q1;
    q1.enqueue(10);
    q1.enqueue(20);
    q1.enqueue(20);
    q1.enqueue(20);
    q1.dequeue();
    q1.dequeue();
    q1.dequeue();
    q1.enqueue(10);
    q1.enqueue(10);
    q1.enqueue(10);

    CustomQueue<int> q2;
    q2 = q1;
    std::cout << q1;
    std::cout << q2;
}
