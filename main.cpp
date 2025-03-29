#include <iostream>
#include <memory>
#include <thread>
#include <vector>
#include "TimerEvent.h"
#include "TaskQueue.h"
#include "TaskQueueManager.h"

int main()
{
    int N = 1;
    std::vector<std::string> worker{"worker1", "worker2", "worker3"};
    TaskQueueManager::GetInstance().get()->CreateTaskQueue(worker);

    TimerEvent event(false, false);

    std::thread threads[N];
    for (int n = 0; n < N; ++n)
    {
        threads[n] = std::thread([&event, &worker]{
            for (int i = 0; i < 10; ++i)
            {
                // TaskQueueManager::GetInstance().get()->GetTaskQueue(worker[0])->AddTask([&event, i](){
                //     std::cout << "task at worker1 queue " << " i = " << i << std::endl; 
                // });

                TaskQueueManager::GetInstance().get()->GetTaskQueue(worker[0])->AddDelayTask([&event, i](){
                    std::cout << "delay task at worker1 queue " << "i = " << i << std::endl;
                }, 1000);

                // TaskQueueManager::GetInstance().get()->GetTaskQueue(worker[1])->AddDelayTask([&event, i](){
                //     std::cout << "dealy task at worker2 queue " << "i = " << i << std::endl;
                //     if (i == 3)
                //     {
                //         std::cout << "event set" << std::endl;
                //         //event.set();
                //     }
                // }, 2000);
            }
        });
    }

    event.wait(-1);

    for (int n = 0; n < N; ++n)
    {
        threads[n].join();
    }
    return 0;
}