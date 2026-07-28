#include "CustomStack.h"
#include <iostream>
#include <string>
#include <cstdlib>

class ResourceLog {
    std::string name;
public:
    ResourceLog(std::string n) : name(n) {
        std::cout << "[생성] " << name << "\n";
    }
    ~ResourceLog() {
        std::cout << "[소멸] " << name << "\n";
    }
};

template <typename T>
class stackLog : public CustomStack<T> //헤더파일 클래스 상속
{
public:
    ~stackLog() {
        std::cout << "[소멸] CustomStack (곧 ~CustomStack() 호출됨)\n";
    }
};

void popExit(bool empty) {
    if (empty) {
        fprintf(stderr, "스택이 비어있음\n");
        exit(1);
    }
}

void doWorkThrow() {
    ResourceLog g1("g1");
    stackLog<int> s;
    ResourceLog g2("g2");
    s.pop();
}

void doWorkExit() {
    ResourceLog g1("g1");
    stackLog<int> s;
    ResourceLog g2("g2");
    popExit(true);
}

int main(int argc, char** argv) {
    std::string mode = (argc > 1) ? argv[1] : "throw"; //터미널에서 무언가 입력 시 입력값 적용, 미입력시 throw 적용

    if (mode == "exit") {
        std::cout << "=== exit() 버전 ===\n";
        doWorkExit();
        std::cout << "(이 줄은 절대 출력되지 않아야 정상)\n";
    } else {
        std::cout << "=== throw 버전 (실제 CustomStack::pop()) ===\n";
        try {
            doWorkThrow();
        } catch (const std::out_of_range& e) {
            std::cout << "[catch] " << e.what() << "\n";
        }
    }
    return 0;
}