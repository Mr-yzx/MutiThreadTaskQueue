#include <thread>
#include "TaskQueueBase.h"

thread_local TaskQueueBase *current = nullptr;

TaskQueueBase* TaskQueueBase::GetCurrentObj()
{
    return current;
}

TaskQueueBase::CurrentTaskQueue::CurrentTaskQueue(TaskQueueBase *pTaskQueue) : m_tmpQueue(current)
{
    current = pTaskQueue;
}

TaskQueueBase::CurrentTaskQueue::~CurrentTaskQueue()
{
    current = m_tmpQueue;
}