#include "CustomDeque.h"
#include <iostream>
#include <stdexcept>

struct CopyAndMove
{
    int value;
    static int copyCount;
    static int moveCount;
    static bool block;
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
    CustomDeque<CopyAndMove> q1;
    q1.add_rear(CopyAndMove(1));
    q1.add_rear(CopyAndMove(2));
    q1.add_rear(CopyAndMove(3));
    q1.add_rear(CopyAndMove(4));

    // 1. 복사 생성자 테스트 (C9)
    CopyAndMove::block = true;
    CopyAndMove::copyCount = 0;
    try
    {
        CustomDeque<CopyAndMove> q2(q1);
        std::cout << "[복사생성자] 예외 안 터짐\n";
    }
    catch (const std::exception &e)
    {
        std::cout << "[복사생성자] 예외 잡힘: " << e.what() << "\n";
    }

    // 2. operator= 테스트 (C9)
    CopyAndMove::block = true;
    CopyAndMove::copyCount = 0;
    CustomDeque<CopyAndMove> q3;
    q3.add_rear(CopyAndMove(100));

    try
    {
        q3 = q1;
        std::cout << "[operator=] 예외 안 터짐\n";
    }
    catch (const std::exception &e)
    {
        std::cout << "[operator=] 예외 잡힘: " << e.what() << "\n";
    }

    std::cout << "[operator= 실패 후 q3 상태] " << q3;

    // 3. 이동 생성자/대입 테스트 (D8)
    CopyAndMove::block = false;
    CopyAndMove::copyCount = 0;
    CopyAndMove::moveCount = 0;

    CustomDeque<CopyAndMove> q4;
    q4.add_rear(CopyAndMove(1));
    q4.add_rear(CopyAndMove(2));

    CustomDeque<CopyAndMove> q5(std::move(q4));
    std::cout << "[이동 생성자] 복사=" << CopyAndMove::copyCount
              << " 이동=" << CopyAndMove::moveCount << " (복사=0, 이동=0 이어야 함)\n";

    CustomDeque<CopyAndMove> q6;
    q6.add_rear(CopyAndMove(100));

    CopyAndMove::copyCount = 0;
    CopyAndMove::moveCount = 0;

    q6 = std::move(q5);

    std::cout << "[이동 대입] 복사=" << CopyAndMove::copyCount
              << " 이동=" << CopyAndMove::moveCount << " (복사=0, 이동=0 이어야 함)\n";
}