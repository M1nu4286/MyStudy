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

    std::cout << "s2: " << s2.peek(); s2.pop();
    std::cout << " " << s2.peek(); s2.pop();
    std::cout << "\n";

    std::cout << "s3: " << s3.peek(); s3.pop();
    std::cout << " " << s3.peek(); s3.pop();
    std::cout << "\n";

    std::cout << "s1: " << s1.peek(); s1.pop();
    std::cout << " " << s1.peek(); s1.pop();
    std::cout << "\n";

    return 0;
}