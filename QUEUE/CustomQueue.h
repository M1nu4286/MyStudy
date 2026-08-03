#ifndef CUSTOM_QUEUE_H
#define CUSTOM_QUEUE_H
#include <stdexcept>
#include <ostream>
template <typename T>
class CustomQueue
{
private:
    T *data;
    int front, rear;
    static constexpr int capacity = 5;
    bool is_full() const
    {
        return ((rear + 1) % capacity == front);
    }
    bool is_empty() const
    {
        return (rear == front);
    }

public:
    CustomQueue()
    {
        front = rear = 0;
        data = new T[capacity];
    }
    ~CustomQueue()
    {
        delete[] data;
    }
    CustomQueue(CustomQueue &&Other) noexcept
        : data(Other.data), front(Other.front), rear(Other.rear)
    {
        Other.data = nullptr;
    }
    CustomQueue& operator=(CustomQueue&& Other) noexcept
{
    if (this != &Other)          // 자기 자신한테 이동 대입하는 경우 방지
    {
        delete[] data;            // 내가 원래 갖고 있던 배열부터 정리
        data = Other.data;         // other의 배열 훔쳐오기
        front = Other.front;
        rear = Other.rear;
        Other.data = nullptr;       // other는 빈손으로 만들기
    }
    return *this;
}
    CustomQueue(const CustomQueue &Other)
    {
        front = Other.front;
        rear = Other.rear;
        data = new T[capacity];
        try
        {
            if (!Other.is_empty())
            {
                int i = Other.front;
                do
                {
                    i = (i + 1) % capacity;
                    data[i] = Other.data[i];
                } while (i != Other.rear);
            }
        }
        catch (...)
        {
            delete[] data;
            throw;
        }
    }
    CustomQueue &operator=(const CustomQueue &Other)
    {
        if (this != &Other)
        {
            T *newData = new T[capacity];

            try
            {
                if (!Other.is_empty())
                {
                    int i = Other.front;
                    do
                    {
                        i = (i + 1) % capacity;
                        newData[i] = Other.data[i]; // ← newData로
                    } while (i != Other.rear);
                }
            }
            catch (...)
            {
                delete[] newData;
                throw;
            }
            delete[] data;  // 기존 거 지우고
            data = newData; // 교체
            front = Other.front;
            rear = Other.rear;
        }
        return *this;
    }
    friend std::ostream &operator<<(std::ostream &os, const CustomQueue<T> &q)
    {
        os << "QUEUE(front=" << q.front << " rear=" << q.rear << ") = ";
        if (!q.is_empty())
        {
            int i = q.front;
            do
            {
                i = (i + 1) % (q.capacity);
                os << q.data[i] << " | ";
                if (i == q.rear)
                    break;
            } while (i != q.front);
        }
        os << "\n";
        return os;
    }

    void enqueue(const T &item)
    {
        if (is_full())
            throw std::out_of_range("큐가 포화상태");
        int tempRear = (rear + 1) % capacity;
        data[tempRear] = item;
        rear = tempRear;
    }
    void dequeue()
    {
        if (is_empty())
            throw std::out_of_range("큐가 공백상태");
        front = (front + 1) % capacity;
    }
    T peek() const
    {
        if (is_empty())
            throw std::out_of_range("큐가 공백상태");
        return data[(front + 1) % capacity]; // 실제 front 원소 반환
    }
};

#endif
