#ifndef CQUEUE_H
#define CQUEUE_H

#include "itemqueue.h"

template <class T> class CQueue
{
private:
	int mSize = 0;
    CItemQueue<T>* mRoot = nullptr;
    CItemQueue<T>* mLast = nullptr;

public:
    CQueue() {};
    ~CQueue()
    {
        CItemQueue<T>* deleted = nullptr;

        while(mRoot != nullptr && mLast != nullptr) {

            deleted = mRoot;

            if(mRoot == mLast)
                mRoot = mLast = nullptr;
            else
                mRoot = mRoot->getNext();

            delete deleted;
            deleted = nullptr;
        }
    }

    void push(const T* data, const int size)
    {
        CItemQueue<T>* item = new CItemQueue<T>(data, size);

        if(mRoot != nullptr && mLast != nullptr) {

            mLast->setNext(item);
            mLast = item;
        } else
            mLast = mRoot = item;
		
		mSize++;
    }

    T* pop()
    {
        CItemQueue<T>* deleted = nullptr;

        if(mRoot != nullptr && mLast != nullptr) {

            deleted = mRoot;

            T* value = nullptr;

            value = mRoot->create(mRoot->getData(), mRoot->getSize());

            if(mRoot == mLast)
                mRoot = mLast = nullptr;
            else
                mRoot = mRoot->getNext();

            delete deleted;
            deleted = nullptr;

			mSize--;

            return value;
        }

        return nullptr;
    }

    const T* top()
    {
        T* value = nullptr;

        if(mRoot != nullptr && mLast != nullptr)
            value = mRoot->getData();

        return value;
    }

	bool available()
    {
        if(mSize != 0)
            return true;
        return false;
    }
};

#endif // CQUEUE_H
