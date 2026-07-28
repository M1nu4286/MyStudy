#include "CustomStack.h"
#include <iostream>
#include <string>

int main() {
    // 1. int — 원본 C 코드와 동일한 타입
    CustomStack<int> s1;
    s1.push(10);
    s1.push(20);
    std::cout << "[int] pop: " << s1.pop() << ", pop: " << s1.pop() << "\n";

    // 2. double — 원본에는 없던 타입, 템플릿이 실제로 일반화됐는지 확인
    CustomStack<double> s2;
    s2.push(3.14);
    s2.push(2.71);
    std::cout << "[double] pop: " << s2.pop() << ", pop: " << s2.pop() << "\n";

    // 3. std::string — 값 타입이 아닌 클래스 타입도 되는지 확인
    CustomStack<std::string> s3;
    s3.push("hello");
    s3.push("world");
    std::cout << "[string] pop: " << s3.pop() << ", pop: " << s3.pop() << "\n";

    // 4. 확장 테스트 — capacity 자동 확장(*2)이 타입 무관하게 동작하는지
    CustomStack<std::string> s4;
    for (int i = 0; i < 5; i++) {
        s4.push("item" + std::to_string(i));
    }
    std::cout << "[string, 확장 후] ";
    while (!s4.is_empty()) {
        std::cout << s4.pop() << " ";
    }
    std::cout << "\n";

    return 0;
}