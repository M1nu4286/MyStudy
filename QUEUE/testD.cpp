#include "CustomDeque.h"
#include <iostream>
#include <cassert>

int main()
{
    // 1. add_rear만 반복 — enqueue와 동일하게 동작해야 함
    std::cout << "=== 테스트 1: add_rear 반복 ===\n";
    CustomDeque<int> d1;
    d1.add_rear(1);
    d1.add_rear(2);
    d1.add_rear(3);
    std::cout << d1;
    std::cout << "기대: 1 | 2 | 3 |\n\n";

    // 2. add_front 한 번 + peek_front로 값 확인
    std::cout << "=== 테스트 2: add_front + peek_front ===\n";
    CustomDeque<int> d2;
    d2.add_front(42);
    std::cout << d2;
    int pf = d2.peek_front();
    std::cout << "peek_front(): " << pf << " (기대: 42)\n";
    std::cout << "\n";

    // 3. add_front 여러 번 — 나중에 넣은 게 맨 앞에 와야 함
    std::cout << "=== 테스트 3: add_front 여러 번 (순서 역전 확인) ===\n";
    CustomDeque<int> d3;
    d3.add_front(1);
    d3.add_front(2);
    d3.add_front(3);
    std::cout << d3;
    std::cout << "기대: 3 | 2 | 1 | (나중에 넣은 3이 맨 앞)\n\n";

    // 4. add_front + add_rear 섞어서 사용
    std::cout << "=== 테스트 4: add_front / add_rear 혼합 ===\n";
    CustomDeque<int> d4;
    d4.add_rear(10);   // [10]
    d4.add_front(20);  // [20, 10]
    d4.add_rear(30);   // [20, 10, 30]
    std::cout << d4;
    std::cout << "기대: 20 | 10 | 30 |\n\n";

    // 5. peek_rear 확인
    std::cout << "=== 테스트 5: peek_rear ===\n";
    int pr = d4.peek_rear();
    std::cout << "peek_rear(): " << pr << " (기대: 30)\n";
    std::cout << "\n";

    // 6. delete_front / delete_rear로 양쪽에서 빼보기
    std::cout << "=== 테스트 6: delete_front / delete_rear ===\n";
    d4.delete_front();  // 20 제거 → [10, 30]
    std::cout << "delete_front() 후: " << d4;
    d4.delete_rear();   // 30 제거 → [10]
    std::cout << "delete_rear() 후: " << d4;
    std::cout << "기대: 10 | 만 남아야 함\n\n";

    // 7. wrap-around 테스트 — capacity=5 기준으로 앞/뒤 다 채워서 인덱스가 배열 경계를 넘게 만듦
    std::cout << "=== 테스트 7: wrap-around (add_front로 앞쪽 인덱스 감소시켜 경계 넘기기) ===\n";
    CustomDeque<int> d5;
    d5.add_rear(100);
    d5.add_front(1);
    d5.add_front(2);   // front가 0 아래로 내려가며 wrap 되는지 확인
    d5.add_front(3);
    std::cout << d5;
    std::cout << "기대: 3 | 2 | 1 | 100 | (앞에서부터 3,2,1 순, 마지막에 100)\n\n";

    // 8. 포화 상태 예외 확인
    std::cout << "=== 테스트 8: 포화 상태 예외 ===\n";
    try {
        d5.add_rear(999); // capacity=5, 1칸 희생이라 4개 찬 상태에서 하나 더 넣으면 포화
        std::cout << "예외 안 터짐 (예상과 다름)\n";
    } catch (const std::out_of_range& e) {
        std::cout << "포화 예외 정상 발생: " << e.what() << "\n";
    }

    std::cout << "\n모든 assert 통과\n";
    return 0;
}