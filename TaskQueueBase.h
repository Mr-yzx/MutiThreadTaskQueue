#ifndef __TASKQUEUEBASE_H__
#define __TASKQUEUEBASE_H__

#include <memory>
#include <string>
#include "Task.h"
/*
/*@brief 任务队列基类
/*
 */
class TaskQueueBase
{
public:
    virtual void DeleteObject() = 0;

    virtual void AddTask(std::unique_ptr<Task> pTask) = 0;

    virtual void AddDelayTask(std::unique_ptr<Task> pTask, uint32_t ms) = 0;

    static TaskQueueBase *GetCurrentObj();

    bool IsCurrentObj()
    {
        return GetCurrentObj() == this;
    }

    virtual std::string GetName() = 0;

protected:
    class CurrentTaskQueue
    {
    public:
        explicit CurrentTaskQueue(TaskQueueBase *pTaskQueue);
        CurrentTaskQueue(const CurrentTaskQueue &) = delete;

        CurrentTaskQueue &operator=(const CurrentTaskQueue &) = delete;
    private:
        TaskQueueBase *m_tmpQueue;
    };

    virtual ~TaskQueueBase() = default;
};

struct TaskQueueDelete
{
    void operator()(TaskQueueBase *pTaskQueue)
    {
        pTaskQueue->DeleteObject();
    }
}
#endif /* __TASKQUEUEBASE_H__ */