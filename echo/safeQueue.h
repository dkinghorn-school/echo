// #ifndef SAFE_QUEUE_H
// #define SAFE_QUEUE_H

// #include <queue>
// #include <mutex>
// #include <condition_variable>

// template <class T>
// class SafeQueue
// {
// public:
//   SafeQueue();
//   void put(T obj);
//   T get();
// private:
//   std::queue<T> list;
//   mutable std::mutex m;
//   std::condition_variable c;
// };

// #endif


#ifndef SAFE_QUEUE
#define SAFE_QUEUE

#include <queue>
#include <mutex>
#include <condition_variable>

template <class T>
class SafeQueue
{
public:
  SafeQueue()//std::mutex* m, std::condition_variable* cv)
  {
    this->m = new std::mutex();
    this->cv = new std::condition_variable();
  }

  ~SafeQueue(void)
  {
    delete cv;
    delete m;
  }

  void put(T t)
  {
    std::lock_guard<std::mutex> lock(*m);
    q.push(t);
    cv->notify_all();
  }

  T get()
  {
    std::unique_lock<std::mutex> lock(*m);
    while(q.empty())
    {
      cv->wait(lock);
    }
    T val = q.front();
    q.pop();
    return val;
  }

private:
  std::queue<T> q;
  mutable std::mutex *m;
  std::condition_variable *cv;
};
#endif