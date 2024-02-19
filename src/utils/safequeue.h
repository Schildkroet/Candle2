#ifndef SAFEQUEUE_H
#define SAFEQUEUE_H


#include <queue>
#include <list>
#include <mutex>
#include <memory>


template<class T>
class SafeQueue
{
public:
    SafeQueue() {}

    void push_back(const T& val)
    {
        std::unique_lock<std::mutex> lock(mMutex);
        mQueue.push_back(val);
    }

    void push_back(T&& val)
    {
        std::unique_lock<std::mutex> lock(mMutex);
        mQueue.push_back(std::move(val));
    }

    void push_front(const T& val)
    {
        std::unique_lock<std::mutex> lock(mMutex);
        mQueue.push_front(val);
    }

    void push_front(T&& val)
    {
        std::unique_lock<std::mutex> lock(mMutex);
        mQueue.push_front(std::move(val));
    }

    T& front()
    {
        std::unique_lock<std::mutex> lock(mMutex);
        return mQueue.front();
    }

    T& back()
    {
        std::unique_lock<std::mutex> lock(mMutex);
        return mQueue.back();
    }

    void pop_front()
    {
        std::unique_lock<std::mutex> lock(mMutex);
        if(!mQueue.empty())
        {
            mQueue.pop_front();
        }
    }

    typename std::list<T>::iterator erase(typename std::list<T>::const_iterator it)
    {
        std::unique_lock<std::mutex> lock(mMutex);
        return mQueue.erase(it);
    }

    typename std::list<T>::iterator get_at(int pos)
    {
        std::unique_lock<std::mutex> lock(mMutex);

        auto it = mQueue.begin();
        std::advance(it, pos);

        return it;
    }

    int size()
    {
        std::unique_lock<std::mutex> lock(mMutex);
        return mQueue.size();
    }

    bool empty() const
    {
        std::unique_lock<std::mutex> lock(mMutex);
        return mQueue.empty();
    }

    void clear()
    {
        mQueue.clear();
    }

private:
   std::list<T> mQueue;
   mutable std::mutex mMutex;
};


#endif // SAFEQUEUE_H
