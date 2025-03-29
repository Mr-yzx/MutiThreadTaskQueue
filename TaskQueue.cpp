#include <memory>
#include <assert.h>
#include "TaskQueue.h"
#include "TimerEvent.h"
#include "TaskQueueBase.h"
TaskQueue::TaskQueue(std::string queueName) :
    m_timerBegin(false, false), m_timerStop(false, false),
    m_notify(false, false), m_name(queueName)
{
    m_thread = std::thread([this]{
        CurrentTaskQueue current(this);
        this->ProcessTask();
    });

    m_timerBegin.wait(TIMER_EVENT_WAIT_FOREVER);
}

void TaskQueue::DeleteObject()
{
    assert(IsCurrentObj() == false);

    std::unique_lock<std::mutex> lock(m_mutex);
    m_quit = true;

    Notify();
    m_timerStop.wait(TIMER_EVENT_WAIT_FOREVER);

    if (m_thread.joinable())
    {
        m_thread.join();
    }

    delete this;
}

void TaskQueue::AddTask(std::unique_ptr<Task> pTask)
{
    std::unique_lock<std::mutex> lock(m_mutex);
    uint64_t order = m_threadOrder++;

    m_taskQueue.push(std::pair<uint64_t, std::unique_ptr<Task>>(order, std::move(pTask)));

    Notify();
}

void TaskQueue::AddDelayTask(std::unique_ptr<Task> pTask, uint64_t ms)
{
    uint64_t expire = GetTimeStamp() + ms;
    DelayTask delayTaskInfo;
    delayTaskInfo.m_expiredTime = expire;

    std::unique_lock<std::mutex> lock(m_mutex);
    delayTaskInfo.m_order = ++m_threadOrder;
    m_delayTaskMap.insert(std::pair<TaskQueue::DelayTask, std::unique_ptr<Task>>(delayTaskInfo, std::move(pTask)));

    Notify();


}

std::string TaskQueue::GetName()
{
    return m_name;
}

void TaskQueue::ProcessTask()
{
    m_timerBegin.set();

    while (true)
    {
        auto task = GetNextTask();
        if (task.m_isFinal)
        {
            break;
        }

        if (task.m_task)
        {
            Task *ptr = task.m_task.release();
            if (ptr->run())
            {
                delete ptr;
            }
            continue;
        }

        if (task.m_sleepTime == 0)
        {
            m_notify.wait(TIMER_EVENT_WAIT_FOREVER);
        }
        else
        {
            m_notify.wait(task.m_sleepTime);
        }
    }
    m_timerStop.set();
}

void TaskQueue::Notify()
{
    m_notify.set();
}

TaskQueue::TaskInfo TaskQueue::GetNextTask()
{
    TaskInfo task;

    uint64_t ts = GetTimeStamp();
    std::unique_lock<std::mutex> lock(m_mutex);

    if (m_quit)
    {
        task.m_isFinal = true;
        return task;
    }

    if (!m_delayTaskMap.empty())
    {
        auto iter = m_delayTaskMap.begin();
        auto &delayInfo = iter->first;
        auto &delayFunc = iter->second;
        if (ts >= delayInfo.m_expiredTime)
        {
            if (m_taskQueue.size() > 0)
            {
                auto &taskInfo = m_taskQueue.front();
                auto &order = taskInfo.first;
                auto &taskFunc = taskInfo.second;
                if (order < delayInfo.m_order)
                {
                    task.m_task = std::move(taskFunc);
                    m_taskQueue.pop();
                    return task;
                }

            }
            task.m_task = std::move(delayFunc);
            m_delayTaskMap.erase(iter);
            return task;
        }

        task.m_sleepTime = delayInfo.m_expiredTime - ts;

    }

    if (m_taskQueue.size() > 0)
    {
        auto &info = m_taskQueue.front();
        task.m_task = std::move(info.second);
        m_taskQueue.pop();
    }
    return task;
}

uint64_t TaskQueue::GetTimeStamp()
{
    return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
}

TaskQueueActually::TaskQueueActually(std::unique_ptr<TaskQueueBase, TaskQueueDelete> taskQueue) : m_taskBase(taskQueue.release())
{

}

TaskQueueActually::~TaskQueueActually()
{   
    m_taskBase->DeleteObject();
}

std::unique_ptr<TaskQueueActually> TaskQueueActually::CreateTaskQueue(std::string name)
{   
    return std::unique_ptr<TaskQueueActually>(new TaskQueueActually(std::unique_ptr<TaskQueueBase, TaskQueueDelete>(new TaskQueue(name))));
}

bool TaskQueueActually::IsCurrentObj()
{
    return m_taskBase->IsCurrentObj();
}

void TaskQueueActually::AddTask(std::unique_ptr<Task> task)
{
    return m_taskBase->AddTask(std::move(task));
}

void TaskQueueActually::AddDelayTask(std::unique_ptr<Task> task, uint64_t ms)
{
    return m_taskBase->AddDelayTask(std::move(task), ms);
}
