#include <sys/time.h>
#include "TimerEvent.h"
using namespace std::chrono;

timespec GetTimeSpec(int pMs)
{
    timespec ts;
    struct timeval tv;
    gettimeofday(&tv, nullptr);
    ts.tv_sec = tv.tv_sec + pMs / 1000;
    ts.tv_nsec = tv.tv_usec * 1000 + (pMs % 1000) * 1000 * 1000;

    if (ts.tv_nsec >= 1000 * 1000 * 1000)
    {
        ts.tv_sec++;
        ts.tv_nsec -= 1000 * 1000 * 1000;
    }

    return ts;

}
TimerEvent::TimerEvent(bool pManualReset, bool pEnable) :
    m_ManualReset(pManualReset), m_enable(pEnable)
{

}

TimerEvent::~TimerEvent()
{
}

void TimerEvent::set()
{
    std::unique_lock<std::mutex> lock(m_mutex);
    m_enable = true;
    m_cond.notify_all();
}

void TimerEvent::reset()
{
    std::unique_lock<std::mutex> lock(m_mutex);
    m_enable = false;
}

bool TimerEvent::wait(int pTimeout, int pWarnTime)
{
    timespec warn_ts, timeout_ts;
    bool warn_flag = (pWarnTime == TIMER_EVENT_WAIT_FOREVER) || 
            (pTimeout != TIMER_EVENT_WAIT_FOREVER && pWarnTime > pTimeout) ?
            false : true;

    bool timeout_flag = (pTimeout == TIMER_EVENT_WAIT_FOREVER) ? false : true;

    if (warn_flag)
    {
        warn_ts = GetTimeSpec(pWarnTime);
        //timeout_ts = GetTimeSpec(pTimeout);
    }

    if (!timeout_flag)
    {
        timeout_ts = GetTimeSpec(pTimeout);
    }

    std::unique_lock<std::mutex> lock(m_mutex);

    auto wait = [&](timespec ts, bool flag)
    {
        std::cv_status status = std::cv_status::no_timeout;
        while (!m_enable && status == std::cv_status::no_timeout)
        {
            if (!flag)
            {
                m_cond.wait(lock);
            }
            else
            {
                system_clock::time_point tp(duration_cast<system_clock::duration>(seconds(ts.tv_sec) + nanoseconds(ts.tv_nsec)));
                status = m_cond.wait_until(lock, tp);
            }
        }
        return status;
    };

    std::cv_status error;
    if (!warn_flag)
    {
        error = wait(timeout_ts, true);
    }
    else
    {
        error = wait(warn_ts, true);
        if (error == std::cv_status::timeout)
        {
            error = wait(timeout_ts, true);
        }
    }

    if (error == std::cv_status::no_timeout && !m_ManualReset)
    {
        m_enable = false;
    }

    return (error == std::cv_status::no_timeout);

}

bool TimerEvent::wait(int pTimeout)
{
    return wait(pTimeout, pTimeout == TIMER_EVENT_WAIT_FOREVER? 3000 : TIMER_EVENT_WAIT_FOREVER);
}
