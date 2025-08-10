#ifndef CITEMQUEUE_H
#define CITEMQUEUE_H

template <class T> class CItemQueue
{
private:
    int mSize = 0;
    T* mData = nullptr;

    CItemQueue* mNext = nullptr;

public:
    CItemQueue(const T* data, const int& size)
        : mSize(size)
    {
        mData = create(data, size);
    }

    ~CItemQueue()
    {
        delete[] mData;
    }

    T* create(const T* data, const int& size)
    {
        T* result = nullptr;

        result = new T[size + 1] { 0 };

        for(int i = 0; i < size; i++)
            result[i] = data[i];

        return result;
    }

    void setNext(CItemQueue* next)
    {
        mNext = next;
    }

    CItemQueue* getNext()
    {
        return mNext;
    }

    T* getData()
    {
        return mData;
    }

    int getSize()
    {
        return mSize;
    }
};

#endif // CITEMQUEUE_H
