#ifndef __TASKQUEUE_H__
#define __TASKQUEUE_H__

#include <string.h>
#include <thread>
#include <mutex>
#include <queue>
#include <memory>
#include <string>
#include <map>
#include <queue>

#include "TaskQueueBase.h"
#include "Task.h"
#include "TimerEvent.h"
class TaskQueue : public TaskQueueBase
{
public:
    TaskQueue(std::string queueName);
    ~TaskQueue() override = default;

    void DeleteObject() override;

    void AddTask(std::unique_ptr<Task> pTask) override;

    void AddDelayTask(std::unique_ptr<Task> pTask, uint64_t ms) override;

    std::string GetName() override;
    
private:
    /*延时任务结构体*/
    struct DelayTask
    {
        uint64_t m_expiredTime;
        uint64_t m_order;       //任务队列的执行顺序

        bool operator<(const DelayTask &other) const
        {
            return std::tie(m_expiredTime, m_order) < std::tie(other.m_expiredTime, other.m_order);
        }
    };

    struct TaskInfo
    {
        bool m_isFinal;
        uint64_t m_sleepTime;
        std::unique_ptr<Task> m_task;
    };

    void ProcessTask();

    void Notify();

    TaskInfo GetNextTask();

    static uint64_t GetTimeStamp();

private:
    std::string m_name;
    std::thread m_thread;
    std::mutex m_mutex;
    std::queue<std::pair<uint64_t, std::unique_ptr<Task>>> m_taskQueue;
    std::map<DelayTask, std::unique_ptr<Task>> m_delayTaskMap;
    
    TimerEvent m_timerBegin;
    TimerEvent m_timerStop;
    TimerEvent m_notify;
    bool m_quit;
    uint64_t m_threadOrder;

};

class TaskQueueActually
{
public:
    explicit TaskQueueActually(std::unique_ptr<TaskQueueBase, TaskQueueDelete> taskQueue);
    ~TaskQueueActually();

    static std::unique_ptr<TaskQueueActually> CreateTaskQueue(std::string name);

    bool IsCurrentObj();

    TaskQueueBase *GetTaskBase()
    {
        return m_taskBase;
    }

    void AddTask(std::unique_ptr<Task> task);

    void AddDelayTask(std::unique_ptr<Task> task, uint64_t ms);

    template <class Closure>
    void AddTask(Closure && closure)
    {
        AddTask(TransferToTask(std::forward<Closure>(closure)));
    }

    template <class Closure>
    void AddDelayTask(Closure && closure, uint64_t ms)
    {
        AddDelayTask(TransferToTask(std::forward<Closure>(closure)), ms);
    }

private:
    TaskQueueActually &operator=(const TaskQueueActually &) = delete;
    TaskQueueActually(const TaskQueueActually &) = delete;   

    TaskQueueBase *m_taskBase;
};



#endif /* __TASKQUEUE_H__ */