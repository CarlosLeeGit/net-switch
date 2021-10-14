#include "netswitch/base/thread_pool.h"

namespace netswitch {
namespace base {

bool JobQueue::Push(const Job &job) {
  std::lock_guard<std::mutex> lock(jobs_lock_);
  if (!working_) {
    return false;
  }
  jobs_.emplace_back(job);
  jobs_notify_.notify_one();
  return true;
}

bool JobQueue::Pop(Job &job) {
  std::unique_lock<std::mutex> lock(jobs_lock_);
  if (!working_) {
    return false;
  }
  jobs_notify_.wait(lock, [this]() { return !IsEmpty() || IsShutdown(); });
  if (IsShutdown()) {
    return false;
  }
  job = std::move(jobs_.front());
  jobs_.pop_front();
  return true;
}

bool JobQueue::IsEmpty() { return jobs_.empty(); }

void JobQueue::Shutdown() {
  std::lock_guard<std::mutex> lock(jobs_lock_);
  working_ = false;
  jobs_notify_.notify_all();
}

bool JobQueue::IsShutdown() { return !working_; }

Worker::Worker(JobQueue *job_queue)
    : job_queue_(job_queue), thread_(&Worker::Run, this) {}

Worker::~Worker() {
  running_ = false;
  thread_.join();
}

void Worker::Run() {
  Job job;
  while (running_) {
    auto ret = job_queue_->Pop(job);
    if (!ret) {
      break;
    }

    job();
  }
}

ThreadPool::ThreadPool(size_t thread_count) {
  workers_.reserve(thread_count);
  for (size_t i = 0; i < thread_count; ++i) {
    workers_.push_back(std::make_shared<Worker>(&job_queue_));
  }
}

ThreadPool::~ThreadPool() {
  job_queue_.Shutdown();
  workers_.clear();
}

ThreadPool &ThreadPool::GetInstance() {
  static ThreadPool instance(40);
  return instance;
}

}  // namespace base
}  // namespace netswitch