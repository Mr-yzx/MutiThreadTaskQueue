#include <iostream>
#include <memory>
#include <thread>
#include <vector>
#include "TimerEvent.h"
#include "TaskQueue.h"
#include "TaskQueueManager.h"

int main()
{
    std::vector<std::string> worker{"woker1", "worker2", "worker3"};
    TaskQueueManager::GetInstance().get()->CreateTaskQueue(worker);

    TimerEvent event(false, false);

    std::thread threads[10];
    for (int n = 0; n < 10; ++n)
    {
        threads[n] = std::thread([&event, &worker]{
            for (int i = 0; i < 10000; ++i)
            {
                TaskQueueManager::GetInstance().get()->GetTaskQueue(worker[0])->AddTask([&event, i](){
                    std::cout << "task at worker1 queue " << " i = " << i << std::endl; 
                });

                TaskQueueManager::GetInstance().get()->GetTaskQueue(worker[0])->AddDelayTask([&event, i](){
                    std::cout << "delay task at worker1 queue " << "i = " << i << std::endl;
                }, 1000);

                TaskQueueManager::GetInstance().get()->GetTaskQueue(worker[1])->AddDelayTask([&event, i](){
                    std::cout << "dealy task at worker2 queue " << "i = " << i << std::endl;
                    if (i == 9999)
                    {
                        static std::mutex mtx;
                        std::lock_guard<std::mutex> lock(mtx);
                        event.set();
                    }
                }, 1000);
            }
        });
    }

    event.wait(-1);

    for (int n = 0; n < 10; ++n)
    {
        threads[n].join();
    }
    return 0;
}