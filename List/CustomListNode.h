#ifndef CUSTOM_LIST_NODE_H
#define CUSTOM_LIST_NODE_H
#include <stdexcept>
#include <ostream>
template <typename T>
class CustomListNode
{
private:
    struct Node
    {
        T data;
        Node *next;
        Node(const T &d) : data(d), next(nullptr) {}
    };
    Node *head;
    int size_ = 0;
    void insert_first(const T &d)
    {
        Node *newNode = new Node(d);
        newNode->next = head;
        head = newNode;
        size_++;
    }
    T remove_first()
    {
        if (empty())
            throw std::out_of_range("리스트가 비어있음");
        T val = head->data;
        Node *temp = head;
        head = head->next;
        delete temp;
        size_--;
        return val;
    }

public:
    CustomListNode() : head(nullptr) {}

    ~CustomListNode()
    {
        Node *cur = head;
        while (cur != nullptr)
        {
            Node *next = cur->next;
            delete cur;
            cur = next;
        }
    }
    CustomListNode(const CustomListNode &) = delete;
    CustomListNode &operator=(const CustomListNode &) = delete;

    friend std::ostream &operator<<(std::ostream &os, const CustomListNode<T> &l)
    {
        os << "List : ";
        Node *cur = l.head;
        while (cur != nullptr)
        {
            os<< cur->data << " -> ";
            cur = cur->next;
        }
        os<< "NULL";
        os << "\n";
        return os;
    }

    int size() const { return size_; }

    bool empty() const { return head == nullptr;}


    void insert(int index, const T &d)
    {
        if (size_ < index || 0 > index)
            throw std::out_of_range("인덱스가 리스트 길이 초과");
        if (index == 0)
            insert_first(d);
        else
        {

            Node *newNode = new Node(d);
            Node *cur = head;
            for (int i = 1; i < index; i++)
            {
                cur = cur->next;
            }
            newNode->next = cur->next;
            cur->next = newNode;
            size_++;
        }
    }

    T remove(int index)
    {
        if (index != 0 && ( size_ <= index || 0 > index))
            throw std::out_of_range("인덱스가 리스트 길이 초과");
        if (index == 0)
            return remove_first();
        else
        {
            Node *cur = head;
            for (int i = 1; i < index; i++)
            {
                cur = cur->next;
            }
            Node *temp = cur->next;
            T val = temp->data;
            cur->next = cur->next->next;
            delete temp;
            size_--;
            return val;
        }
    }
};

#endif