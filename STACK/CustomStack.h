#ifndef CUSTOM_STACK_H
#define CUSTOM_STACK_H

#include <stdexcept>

template <typename T>
class CustomStack
{
private:
    T *data;
    int top;
    int capacity;

public:
    // 생성자: 초기 capacity 1로 시작 (원본 로직 그대로 유지)
    CustomStack()
    {
        top = -1;
        capacity = 1;
        data = new T[capacity];
    };
    CustomStack(const CustomStack &Other)
    {
        top = Other.top;
        capacity = Other.capacity;
        data = new T[capacity];
        for (int i = 0; i <= top; i++)
        {
            data[i] = Other.data[i];
        }
    }
    CustomStack &operator=(const CustomStack &Other)
    {
        if (this != &Other)
        {
            delete[] data;
            top = Other.top;
            capacity = Other.capacity;
            data = new T[capacity];
            for (int i = 0; i <= top; i++)
            {
                data[i] = Other.data[i];
            }
        }
        return *this;
    }
    // 소멸자: data 해제
    ~CustomStack()
    {
        delete[] data;
    };

    void push(const T &item)
    {
        if (is_full())
        {
            int newCapacity = capacity * 2;
            T *newData = new T[newCapacity]; // 새 배열
            for (int i = 0; i <= top; i++)
            {
                newData[i] = data[i]; // 기존 데이터 복사
            }
            delete[] data;  // 기존 배열 해제
            data = newData; // 교체
            capacity = newCapacity;
        }
        data[++top] = item;
    };
    T pop()
    {
        if (is_empty())
            throw std::out_of_range("스택이 비어있음");
        return data[top--];
    };
    T peek() const
    {
        if (is_empty())
            throw std::out_of_range("스택이 비어있음");
        return data[top];
    };
    bool is_empty() const { return (top == -1); };
    bool is_full() const { return (top == capacity - 1); };
};

#endif