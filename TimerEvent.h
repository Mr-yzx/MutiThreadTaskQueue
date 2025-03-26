#ifndef __TIMEREVENT_H__
#define __TIMEREVENT_H__

#include <mutex>
#include <condition_variable>
#include <stdint.h>

#define TIMER_EVENT_WAIT_FOREVER (-1)
/*
/*@brief 用于处理延时时间的类
/*
 */

class TimerEvent
{
public:
    TimerEvent();

    TimerEvent(bool pManualReset, bool pEnable);

    ~TimerEvent();

    void set();

    void reset();

    /*
    /*@brief 延时事件函数
    /*
    /*@param timeout 延时时间
    /*@param warn_time 警告时间
    /*@return true 
    /*@return false 
     */
    bool wait(int pTimeout, int pWarnTime);

    /*
    /*@brief 等待延时事件发生的入口函数
    /*
    /*@param timeout 延时时间，单位 ms
    /*@return true 
    /*@return false 
     */
    bool wait(int pTimeout);

    /*
    /*@brief 禁用拷贝构造和赋值构造
    /*
     */
    TimerEvent(const TimerEvent &) = delete;
    TimerEvent &operator=(const TimerEvent &) = delete;
private:
    std::mutex m_mutex;
    std::condition_variable m_cond;
    bool m_ManualReset;
    bool m_enable;
};

#endif /* __TIMEREVENT_H__ */