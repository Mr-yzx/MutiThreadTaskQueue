#include "TaskQueueManager.h"
#include "TaskQueue.h"
std::unique_ptr<TaskQueueManager> &TaskQueueManager::GetInstance()
{
    static std::unique_ptr<TaskQueueManager> instance = std::unique_ptr<TaskQueueManager>(new TaskQueueManager());
    
    return instance;
}

TaskQueueManager::~TaskQueueManager()
{
    clear();
}

void TaskQueueManager::CreateTaskQueue(std::vector<std::string> &list)
{
    std::unique_lock<std::mutex> lock(m_mutex);
    for (auto &name : list)
    {
        if (!exist(name))
        {
            m_taskQueueMap[name] = TaskQueueActually::CreateTaskQueue(name);

        }
    }
}

TaskQueueActually *TaskQueueManager::GetTaskQueue(std::string &name)
{
    std::unique_lock<std::mutex> lock(m_mutex);
    return exist(name) ? m_taskQueueMap[name].get() : nullptr;
}

bool TaskQueueManager::IsQueueExist(std::string &name)
{
    std::unique_lock<std::mutex> lock(m_mutex);
    return exist(name);
}

void TaskQueueManager::clear()
{
    std::unique_lock<std::mutex> lock(m_mutex);
    m_taskQueueMap.clear();
}

bool TaskQueueManager::exist(std::string &name)
{
    return (m_taskQueueMap.find(name) != m_taskQueueMap.end());
}

TaskQueueManager::TaskQueueManager()
{
    
}
