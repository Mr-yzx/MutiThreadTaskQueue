#ifndef __TASK_H__
#define __TASK_H__

#include <type_traits>
#include <memory>

/*
/*@brief 具体任务基类
/*
 */
class Task
{
public:
    virtual ~Task() = default;

    virtual void run() = 0;
};

/*
/*@brief 闭包任务类，继承自基类
/*
/*@tparam Closure 
 */
template <typename Closure>
class TaskClosure : public Task
{
public:
    explicit TaskClosure(Closure &&pClosure) : m_closure(std::forawrd<Closure>(pClosure))
    {

    }

private:
    void run() override
    {
        m_closure();
    }
    typename std::decay<Closure>::type m_closure;
};

/*
/*@brief 清空任务的闭包类
/*
/*@tparam Closure 
/*@tparam Clearup 
 */
template <typename Closure, typename Clearup>
class TaskClosureClearup : public TaskClosure<Closure>
{
public:
    TaskClosureClearup(Closure &&pClosure, Clearup &&pClearup) :
        TaskClosure<Closure>(std::forward<Closure>(pClosure)),
        m_clearup(std::forward<Clearup>(pClearup))
    {

    }

    ~TaskClosureClearup() override
    {
        m_clearup();
    }
private:
    typename std::decay<Clearup>::type m_clearup;
};


template <typename Closure>
std::unique_ptr<Task> TransferToTask(Closure &&pClosure)
{
    return std::unique_ptr<TaskClosure<Closure>>(std::forward<Closure>(pClosure));
}
#endif /* __TASK_H__ */