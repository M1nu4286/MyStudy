#ifndef CUSTOM_DEQUE_H
#define CUSTOM_DEQUE_H
#include <stdexcept>
#include <ostream>
template <typename T>
class CustomDeque
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
    CustomDeque()
    {
        front = rear = 0;
        data = new T[capacity];
    }
    ~CustomDeque()
    {
        delete[] data;
    }
    CustomDeque(CustomDeque &&Other) noexcept
        : data(Other.data), front(Other.front), rear(Other.rear)
    {
        Other.data = nullptr;
    }
    CustomDeque &operator=(CustomDeque &&Other) noexcept
    {
        if (this != &Other) // 자기 자신한테 이동 대입하는 경우 방지
        {
            delete[] data;     // 내가 원래 갖고 있던 배열부터 정리
            data = Other.data; // other의 배열 훔쳐오기
            front = Other.front;
            rear = Other.rear;
            Other.data = nullptr; // other는 빈손으로 만들기
        }
        return *this;
    }
    CustomDeque(const CustomDeque &Other)
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
    CustomDeque &operator=(const CustomDeque &Other)
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
    friend std::ostream &operator<<(std::ostream &os, const CustomDeque<T> &q)
    {
        os << "DEQUE(front=" << q.front << " rear=" << q.rear << ") = ";
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

    void add_front(const T &item)
    {
        if (is_full())
            throw std::out_of_range("데크가 포화상태");
        data[front] = item;
        front = (front - 1 + capacity) % capacity;
    }
    void add_rear(const T &item)
    {
        if (is_full())
            throw std::out_of_range("데크가 포화상태");
        int tempRear = (rear + 1) % capacity;
        data[tempRear] = item;
        rear = tempRear;
    }
    void delete_front()
    {
        if (is_empty())
            throw std::out_of_range("데크가 공백상태");
        front = (front + 1) % capacity;
    }
    void delete_rear()
    {
        if (is_empty())
            throw std::out_of_range("데크가 공백상태");
        rear = (rear - 1 + capacity) % capacity;
    }
    T peek_front() const
    {
        if (is_empty())
            throw std::out_of_range("데크가 공백상태");
        return data[(front + 1) % capacity]; // 실제 front 원소 반환
    }
    T peek_rear() const
    {
        if (is_empty())
            throw std::out_of_range("데크가 공백상태");
        return data[rear];
    }
};

#endif
