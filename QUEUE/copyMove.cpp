#include "CustomQueue.h"
#include <iostream>
#include <stdexcept>

struct CopyAndMove
{
    int value;            // 단순 값
    static int copyCount; // 몇번 복사했는지
    static int moveCount;
    static bool block; // 안전장치
    CopyAndMove() : value(0) {}
    CopyAndMove(int v) : value(v) {}
    CopyAndMove(const CopyAndMove &Other) : value(Other.value) { copyCount++; }
    CopyAndMove(CopyAndMove &&Other) noexcept : value(Other.value) { moveCount++; }
    CopyAndMove &operator=(const CopyAndMove &Other)
    {
        if (block && ++copyCount == 3)
            throw std::runtime_error("일부러 3번째 대입에서 터뜨림");
        value = Other.value;
        return *this;
    }
    CopyAndMove &operator=(CopyAndMove &&Other) noexcept
    {
        value = Other.value;
        moveCount++;
        return *this;
    }
    friend std::ostream &operator<<(std::ostream &os, const CopyAndMove &t)
    {
        os << t.value;
        return os;
    }
};
int CopyAndMove::copyCount = 0;
int CopyAndMove::moveCount = 0;
bool CopyAndMove::block = false;

int main()
{
    CustomQueue<CopyAndMove> q1;
    q1.enqueue(CopyAndMove(1));
    q1.enqueue(CopyAndMove(2));
    q1.enqueue(CopyAndMove(3));
    q1.enqueue(CopyAndMove(4));

    // 1. 복사 생성자 테스트
    CopyAndMove::block = true;
    CopyAndMove::copyCount = 0;
    try
    {
        CustomQueue<CopyAndMove> q2(q1);
        std::cout << "[복사생성자] 예외 안 터짐\n";
    }
    catch (const std::exception &e)
    {
        std::cout << "[복사생성자] 예외 잡힘: " << e.what() << "\n";
    }

    // 2. operator= 테스트 —
    CopyAndMove::block = true;
    CopyAndMove::copyCount = 0;
    CustomQueue<CopyAndMove> q3;
    q3.enqueue(CopyAndMove(100)); // 대입 실패해도 이게 살아있어야 함

    try
    {
        q3 = q1;
        std::cout << "[operator=] 예외 안 터짐\n";
    }
    catch (const std::exception &e)
    {
        std::cout << "[operator=] 예외 잡힘: " << e.what() << "\n";
    }

    // q3가 대입 실패 후에도 원래 데이터(100)를 그대로 갖고 있어야 한다
    std::cout << "[operator= 실패 후 q3 상태] " << q3;

    CopyAndMove::block = false;
    CopyAndMove::copyCount = 0;
    CopyAndMove::moveCount = 0;

    CustomQueue<CopyAndMove> q4;
    q4.enqueue(CopyAndMove(1));
    q4.enqueue(CopyAndMove(2));

    CustomQueue<CopyAndMove> q5(std::move(q4));
    std::cout << "[이동 생성자] 복사=" << CopyAndMove::copyCount
              << " 이동=" << CopyAndMove::moveCount << " (복사는 0이어야 함)\n";

    CustomQueue<CopyAndMove> q6;
    q6.enqueue(CopyAndMove(100)); 

    CopyAndMove::copyCount = 0;
    CopyAndMove::moveCount = 0;

    q6 = std::move(q5); // 이동 대입 연산자 호출

    std::cout << "[이동 대입] 복사=" << CopyAndMove::copyCount
              << " 이동=" << CopyAndMove::moveCount << " (복사는 0이어야 함)\n";
}