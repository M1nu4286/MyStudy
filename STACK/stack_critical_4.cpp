// Critical 4: 템플릿 미적용 → 템플릿화 검증
// pop()이 void로 변경됨에 따라 peek()+pop() 두 단계로 값 확인

#include "CustomStack.h"
#include <iostream>
#include <string>

int main() {
    // 1. int
    CustomStack<int> s1;
    s1.push(10);
    s1.push(20);
    std::cout << "[int] peek: " << s1.peek();
    s1.pop();
    std::cout << ", peek: " << s1.peek();
    s1.pop();
    std::cout << "\n";

    // 2. double
    CustomStack<double> s2;
    s2.push(3.14);
    s2.push(2.71);
    std::cout << "[double] peek: " << s2.peek();
    s2.pop();
    std::cout << ", peek: " << s2.peek();
    s2.pop();
    std::cout << "\n";

    // 3. std::string
    CustomStack<std::string> s3;
    s3.push("hello");
    s3.push("world");
    std::cout << "[string] peek: " << s3.peek();
    s3.pop();
    std::cout << ", peek: " << s3.peek();
    s3.pop();
    std::cout << "\n";

    // 4. 확장 테스트
    CustomStack<std::string> s4;
    for (int i = 0; i < 5; i++) {
        s4.push("item" + std::to_string(i));
    }
    std::cout << "[string, 확장 후] ";
    while (!s4.is_empty()) {
        std::cout << s4.peek() << " ";
        s4.pop();
    }
    std::cout << "\n";

    return 0;
}