#ifndef __TASKQUEUEMANAGER_H__
#define __TASKQUEUEMANAGER_H__

#include <memory>
#include <string>
#include <vector>
#include <unordered_map>
#include <mutex>


class TaskQueueActually;

class TaskQueueManager
{
public:
    static std::unique_ptr<TaskQueueManager> &GetInstance();

    ~TaskQueueManager();

    void CreateTaskQueue(std::vector<std::string> &list);

    TaskQueueActually *GetTaskQueue(std::string &name);

    bool IsQueueExist(std::string &name);

private:
    void clear();

    bool exist(std::string &name);

private:
    TaskQueueManager();

    TaskQueueManager(TaskQueueManager &&) = delete;

    TaskQueueManager(const TaskQueueManager &) = delete;

    TaskQueueManager &operator=(const TaskQueueManager &) = delete;

private:
    std::mutex m_mutex;
    std::unordered_map<std::string, std::unique_ptr<TaskQueueActually>> m_taskQueueMap;

    //static std::unique_ptr<TaskQueueManager> &m_instance;
};

#endif /* __TASKQUEUEMANAGER_H__ */