#include "FTthreadManager.h"
#include <iostream>
#include <algorithm>

FTthreadManager::FTthreadManager() : 
    m_max_running(5),
    m_online(true) 
{
    init_runner_queue();
    m_scheduler_thread = std::thread(&FTthreadManager::start_scheduler, this);
}

FTthreadManager::FTthreadManager( int max_running ) : 
    m_max_running(max_running),
    m_online(true)
{
    init_runner_queue();
    m_scheduler_thread = std::thread(&FTthreadManager::start_scheduler, this);
}

FTthreadManager::~FTthreadManager()
{
    m_online = false;
    m_scheduler_thread.join();
}

void FTthreadManager::init_runner_queue()
{
    m_task_queue_running.resize( m_max_running );
    for( auto & task : m_task_queue_running ) {
        if( task.active ) {
            if( task.future.wait_for(std::chrono::seconds(0)) == std::future_status::ready ) {
                task.active = false;
                task.promise = std::promise<bool>();
                task.future = task.promise.get_future();
            }
        }
    }
}
void FTthreadManager::start_scheduler() 
{
    while( m_online ) {
        for( auto & task : m_task_queue_running ) {
            if( task.active ) {
                if( task.future.wait_for(std::chrono::seconds(0)) == std::future_status::ready ) {
                    m_mutex.lock();
                    task.active = false;
                    m_mutex.unlock();
                }
            }
        }
        if( m_task_queue_pending.size() > 0 ) {
            auto it = find_if( m_task_queue_running.begin(), m_task_queue_running.end(), [this]( task_t & task )
                    {
                        return !task.active;
                    });
            if( it != m_task_queue_running.end() ) { 
                m_mutex.lock();
                it->active = true;
                it->promise = std::promise<bool>();
                it->future = it->promise.get_future();
                std::thread th( &FTthreadManager::task_wrapper, this, m_task_queue_pending.front(), std::ref(it->promise) );
                th.detach();
                m_task_queue_pending.pop();
                m_mutex.unlock();
            }
        }
        std::this_thread::yield();
    }
}

void FTthreadManager::task_wrapper( std::function<void()> f, std::promise<bool> & p )
{
    f();
    m_mutex.lock();
    p.set_value(true);
    m_mutex.unlock();
}
        
void FTthreadManager::synchronize()
{
    auto it = m_task_queue_running.begin();
    while( (m_task_queue_pending.size() > 0) || (it != m_task_queue_running.end()) ) {
        it = find_if( m_task_queue_running.begin(), m_task_queue_running.end(), [this]( task_t & task )
            {
                return task.active;
            });
        std::this_thread::yield();
    }
}
