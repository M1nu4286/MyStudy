#include "CustomStack.h"
#include <iostream>

int main() {
    CustomStack<int> s1;
    s1.push(1);
    s1.push(2);

    CustomStack<int> s2 = s1;   // 복사 생성자
    CustomStack<int> s3;
    s3 = s1;                     // 복사 대입 연산자
    s1 = s1;                     // self-assignment 방어 확인

    std::cout << "s2: " << s2.pop() << " " << s2.pop() << "\n";
    std::cout << "s3: " << s3.pop() << " " << s3.pop() << "\n";
    std::cout << "s1: " << s1.pop() << " " << s1.pop() << "\n";

    return 0;  // s1, s2, s3 전부 소멸 — valgrind로 double free 없는지 확인
}