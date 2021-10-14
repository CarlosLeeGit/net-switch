#ifndef NET_SWITCH_THREAD_POOL_H_
#define NET_SWITCH_THREAD_POOL_H_

#include <atomic>
#include <condition_variable>
#include <functional>
#include <future>
#include <list>
#include <memory>
#include <mutex>
#include <thread>
#include <vector>

namespace netswitch {
namespace base {

class ThreadPool;

using Job = std::function<void()>;

class JobQueue {
 public:
  bool Push(const Job &job);

  bool Pop(Job &job);

  bool IsEmpty();

  void Shutdown();

  bool IsShutdown();

 private:
  std::mutex jobs_lock_;
  std::list<Job> jobs_;
  std::condition_variable jobs_notify_;
  bool working_{true};
};

class Worker {
 public:
  Worker(JobQueue *job_queue);

  virtual ~Worker();

  void Run();

 private:
  JobQueue *job_queue_;
  std::atomic_bool running_{true};
  std::thread thread_;
};

class ThreadPool {
 public:
  ThreadPool(size_t thread_count);

  virtual ~ThreadPool();

  template <typename Func, typename... ARG>
  auto Submit(const Func &func, ARG &&... args)
      -> std::future<typename std::result_of<Func(ARG...)>::type> {
    auto job = std::bind(func, std::forward<ARG>(args)...);
    using Type = typename std::result_of<Func(ARG...)>::type;
    auto task = std::make_shared<std::packaged_task<Type()>>(std::move(job));
    auto result = task->get_future();
    auto executor = [task]() { (*task)(); };
    job_queue_.Push(std::move(executor));
    return result;
  };

  static ThreadPool &GetInstance();

 private:
  JobQueue job_queue_;
  std::vector<std::shared_ptr<Worker>> workers_;
};

}  // namespace base
}  // namespace netswitch

#endif  // NET_SWITCH_THREAD_POOL_H_