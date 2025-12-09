#pragma once
#include <queue>
#include <functional>
#include <memory>
#include <vector>
#include <map>
#include <string>
#include <chrono>
#include <mutex>
#include <condition_variable>
#include <atomic>

// ===== Async Runtime for SynthFlow =====
// Provides async/await functionality

// Forward declarations
class AsyncRuntime;
class Future;

// Promise state
enum class PromiseState {
    PENDING,
    FULFILLED,
    REJECTED
};

// Callback type
using TaskCallback = std::function<void()>;
using ResolveCallback = std::function<void(std::shared_ptr<void>)>;
using RejectCallback = std::function<void(std::string)>;

// Timer entry
struct Timer {
    int64_t id;
    std::chrono::steady_clock::time_point fireTime;
    TaskCallback callback;
    bool repeating;
    std::chrono::milliseconds interval;
    
    Timer() : id(0), repeating(false) {}
    
    bool operator>(const Timer& other) const {
        return fireTime > other.fireTime;
    }
};

// Promise/Future for async operations
template<typename T>
class Promise {
private:
    PromiseState state;
    T value;
    std::string error;
    std::vector<std::function<void(T)>> thenCallbacks;
    std::vector<std::function<void(std::string)>> catchCallbacks;
    mutable std::mutex mutex;
    
public:
    Promise() : state(PromiseState::PENDING) {}
    
    void resolve(T val) {
        std::lock_guard<std::mutex> lock(mutex);
        if (state != PromiseState::PENDING) return;
        
        state = PromiseState::FULFILLED;
        value = val;
        
        for (auto& callback : thenCallbacks) {
            callback(value);
        }
    }
    
    void reject(const std::string& err) {
        std::lock_guard<std::mutex> lock(mutex);
        if (state != PromiseState::PENDING) return;
        
        state = PromiseState::REJECTED;
        error = err;
        
        for (auto& callback : catchCallbacks) {
            callback(error);
        }
    }
    
    template<typename U>
    std::shared_ptr<Promise<U>> then(std::function<U(T)> onFulfilled) {
        auto next = std::make_shared<Promise<U>>();
        
        std::lock_guard<std::mutex> lock(mutex);
        if (state == PromiseState::FULFILLED) {
            try {
                next->resolve(onFulfilled(value));
            } catch (const std::exception& e) {
                next->reject(e.what());
            }
        } else if (state == PromiseState::PENDING) {
            thenCallbacks.push_back([next, onFulfilled](T val) {
                try {
                    next->resolve(onFulfilled(val));
                } catch (const std::exception& e) {
                    next->reject(e.what());
                }
            });
        }
        
        return next;
    }
    
    std::shared_ptr<Promise<T>> catchError(std::function<void(std::string)> onRejected) {
        std::lock_guard<std::mutex> lock(mutex);
        if (state == PromiseState::REJECTED) {
            onRejected(error);
        } else if (state == PromiseState::PENDING) {
            catchCallbacks.push_back(onRejected);
        }
        return std::shared_ptr<Promise<T>>(this, [](Promise<T>*){});
    }
    
    PromiseState getState() const {
        std::lock_guard<std::mutex> lock(mutex);
        return state;
    }
    
    T getValue() const {
        std::lock_guard<std::mutex> lock(mutex);
        return value;
    }
    
    std::string getError() const {
        std::lock_guard<std::mutex> lock(mutex);
        return error;
    }
    
    bool isPending() const { return getState() == PromiseState::PENDING; }
    bool isFulfilled() const { return getState() == PromiseState::FULFILLED; }
    bool isRejected() const { return getState() == PromiseState::REJECTED; }
};

// Task in the event queue
struct AsyncTask {
    TaskCallback callback;
    int priority;
    
    AsyncTask() : priority(0) {}
    AsyncTask(TaskCallback cb, int prio = 0) : callback(std::move(cb)), priority(prio) {}
    
    bool operator<(const AsyncTask& other) const {
        return priority < other.priority;  // Higher priority first
    }
};

// Event loop / Async runtime
class AsyncRuntime {
private:
    std::priority_queue<AsyncTask> taskQueue;
    std::priority_queue<Timer, std::vector<Timer>, std::greater<Timer>> timerQueue;
    std::atomic<bool> running;
    std::atomic<int64_t> nextTimerId;
    std::mutex queueMutex;
    std::condition_variable queueCondition;
    
public:
    AsyncRuntime() : running(false), nextTimerId(1) {}
    
    // Schedule a task to run on the event loop
    void schedule(TaskCallback callback, int priority = 0) {
        std::lock_guard<std::mutex> lock(queueMutex);
        taskQueue.push(AsyncTask(std::move(callback), priority));
        queueCondition.notify_one();
    }
    
    // Schedule a microtask (high priority)
    void scheduleMicrotask(TaskCallback callback) {
        schedule(std::move(callback), 100);  // Higher priority
    }
    
    // Set a timeout
    int64_t setTimeout(TaskCallback callback, int milliseconds) {
        std::lock_guard<std::mutex> lock(queueMutex);
        
        Timer timer;
        timer.id = nextTimerId++;
        timer.fireTime = std::chrono::steady_clock::now() + 
                         std::chrono::milliseconds(milliseconds);
        timer.callback = std::move(callback);
        timer.repeating = false;
        
        timerQueue.push(timer);
        queueCondition.notify_one();
        
        return timer.id;
    }
    
    // Set an interval
    int64_t setInterval(TaskCallback callback, int milliseconds) {
        std::lock_guard<std::mutex> lock(queueMutex);
        
        Timer timer;
        timer.id = nextTimerId++;
        timer.fireTime = std::chrono::steady_clock::now() + 
                         std::chrono::milliseconds(milliseconds);
        timer.callback = std::move(callback);
        timer.repeating = true;
        timer.interval = std::chrono::milliseconds(milliseconds);
        
        timerQueue.push(timer);
        queueCondition.notify_one();
        
        return timer.id;
    }
    
    // Run the event loop
    void run() {
        running = true;
        
        while (running) {
            std::unique_lock<std::mutex> lock(queueMutex);
            
            // Process expired timers
            auto now = std::chrono::steady_clock::now();
            while (!timerQueue.empty() && timerQueue.top().fireTime <= now) {
                Timer timer = timerQueue.top();
                timerQueue.pop();
                
                // Execute timer callback
                lock.unlock();
                timer.callback();
                lock.lock();
                
                // Reschedule if repeating
                if (timer.repeating) {
                    timer.fireTime = now + timer.interval;
                    timerQueue.push(timer);
                }
            }
            
            // Process tasks
            if (!taskQueue.empty()) {
                AsyncTask task = taskQueue.top();
                taskQueue.pop();
                
                lock.unlock();
                task.callback();
                lock.lock();
            } else if (timerQueue.empty()) {
                // No tasks or timers - wait
                queueCondition.wait(lock);
            } else {
                // Wait until next timer
                queueCondition.wait_until(lock, timerQueue.top().fireTime);
            }
        }
    }
    
    // Run one iteration of the event loop
    bool runOnce() {
        std::unique_lock<std::mutex> lock(queueMutex);
        
        // Process expired timers
        auto now = std::chrono::steady_clock::now();
        while (!timerQueue.empty() && timerQueue.top().fireTime <= now) {
            Timer timer = timerQueue.top();
            timerQueue.pop();
            
            lock.unlock();
            timer.callback();
            lock.lock();
            
            if (timer.repeating) {
                timer.fireTime = now + timer.interval;
                timerQueue.push(timer);
            }
        }
        
        // Process one task
        if (!taskQueue.empty()) {
            AsyncTask task = taskQueue.top();
            taskQueue.pop();
            
            lock.unlock();
            task.callback();
            return true;
        }
        
        return !timerQueue.empty();
    }
    
    // Run until all tasks complete
    void runUntilComplete() {
        while (runOnce()) {}
    }
    
    // Stop the event loop
    void stop() {
        running = false;
        queueCondition.notify_all();
    }
    
    // Check if running
    bool isRunning() const {
        return running;
    }
    
    // Check if there are pending tasks
    bool hasPendingTasks() const {
        return !taskQueue.empty() || !timerQueue.empty();
    }
};

// Async function wrapper (simulates async/await)
template<typename T>
class AsyncFunction {
private:
    std::function<std::shared_ptr<Promise<T>>()> implementation;
    
public:
    AsyncFunction(std::function<std::shared_ptr<Promise<T>>()> impl)
        : implementation(std::move(impl)) {}
    
    std::shared_ptr<Promise<T>> call() {
        return implementation();
    }
    
    // Await - blocks until promise resolves (for synchronous contexts)
    T await() {
        auto promise = call();
        while (promise->isPending()) {
            // In real implementation, would yield to event loop
            std::this_thread::yield();
        }
        if (promise->isRejected()) {
            throw std::runtime_error(promise->getError());
        }
        return promise->getValue();
    }
};

// Helper to create a resolved promise
template<typename T>
std::shared_ptr<Promise<T>> resolvedPromise(T value) {
    auto promise = std::make_shared<Promise<T>>();
    promise->resolve(value);
    return promise;
}

// Helper to create a rejected promise
template<typename T>
std::shared_ptr<Promise<T>> rejectedPromise(const std::string& error) {
    auto promise = std::make_shared<Promise<T>>();
    promise->reject(error);
    return promise;
}

// Global async runtime instance
inline AsyncRuntime& getAsyncRuntime() {
    static AsyncRuntime instance;
    return instance;
}

// ===== SynthFlow Async Built-ins =====
// These would be exposed to SynthFlow code:

// async fn fetch(url: string) -> string
// let data = await fetch("https://api.example.com/data")

// async fn delay(ms: int) -> void
// await delay(1000)  // Sleep for 1 second

// async fn readFileAsync(path: string) -> string
// let content = await readFileAsync("data.txt")
