#include "CustomListNode.h"
#include <iostream>

int main() {
    CustomListNode<int> list;

    // 1. 빈 리스트 remove(0) -> 예외 확인
    std::cout << "[1] 빈 리스트 remove(0)\n";
    try {
        list.remove(0);
    } catch (const std::out_of_range& e) {
        std::cout << "  예외 발생(정상): " << e.what() << "\n";
    }

    // 2. insert(0, ...) 반복 -> head 삽입
    std::cout << "\n[2] insert(0, ...) x3 (head 삽입)\n";
    list.insert(0, 10);
    list.insert(0, 20);
    list.insert(0, 30);
    std::cout << "  " << list;   // 예상: 30 -> 20 -> 10
    std::cout << "  size(): " << list.size() << "\n";

    // 3. insert(size(), ...) -> append
    std::cout << "\n[3] insert(size(), 99) (append)\n";
    list.insert(list.size(), 99);
    std::cout << "  " << list;   // 예상: 30 -> 20 -> 10 -> 99

    // 4. 중간 삽입
    std::cout << "\n[4] insert(2, 555) (중간 삽입)\n";
    list.insert(2, 555);
    std::cout << "  " << list;   // 예상: 30 -> 20 -> 555 -> 10 -> 99

    // 5. 중간 삭제
    std::cout << "\n[5] remove(2) (중간 삭제)\n";
    int removed = list.remove(2);
    std::cout << "  삭제된 값: " << removed << "\n";
    std::cout << "  " << list;   // 예상: 30 -> 20 -> 10 -> 99

    // 6. 범위 밖 index -> 예외 확인
    std::cout << "\n[6] 범위 밖 index 예외 확인\n";
    try {
        list.insert(list.size() + 1, 0);
    } catch (const std::out_of_range& e) {
        std::cout << "  insert 범위 초과 예외(정상): " << e.what() << "\n";
    }
    try {
        list.remove(list.size());
    } catch (const std::out_of_range& e) {
        std::cout << "  remove 범위 초과 예외(정상): " << e.what() << "\n";
    }

    // 7. 마지막 원소까지 전부 제거 -> empty() 확인
    std::cout << "\n[7] 전부 제거 후 empty() 확인\n";
    while (!list.empty()) {
        int val = list.remove(0);
        std::cout << "  remove(0) -> " << val << " | " << list;
    }
    std::cout << "  empty(): " << (list.empty() ? "true" : "false") << "\n";
    std::cout << "  size(): " << list.size() << "\n";

    return 0;
}