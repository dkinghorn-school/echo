
// #include "safeQueue.h"

// template <class T>
// SafeQueue<T>::SafeQueue()
// {
//   this->m = new std::mutex();
//   this->cv = new std::condition_variable();
// }

// template <class T>
// SafeQueue<T>::~SafeQueue()
// {
//   delete cv;
//   delete m;
// }

// template <class T>
// void SafeQueue<T>::put(T t)
// {
//   std::lock_guard<std::mutex> lock(*m);
//   q.push(t);
//   cv->notify_all();
// }

// template <class T>
// T SafeQueue<T>::get()
// {
//   std::unique_lock<std::mutex> lock(*m);
//   while (q.empty())
//   {
//     cv->wait(lock);
//   }
//   T val = q.front();
//   q.pop();
//   return val;
// }
