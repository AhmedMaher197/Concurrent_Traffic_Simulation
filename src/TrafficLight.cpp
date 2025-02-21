#include <iostream>
#include <random>
#include "TrafficLight.h"

template <typename T>
T MessageQueue<T>::receive()
{
    std::unique_lock<std::mutex> lock(_mtx);
    _condition.wait(lock, [this]() { return !_queue.empty(); });

    T msg = std::move(_queue.back());

    return msg;
}

template <typename T>
void MessageQueue<T>::send(T &&msg)
{
    std::lock_guard<std::mutex> lock(_mtx);
    _queue.push_back(std::move(msg));
    _condition.notify_one();
}

TrafficLight::TrafficLight() : _currentPhase {TrafficLightPhase::red}
{   
}

void TrafficLight::waitForGreen()
{
    while (true) 
    {
     	if (_messageQueue.receive() == TrafficLightPhase::green) 
        {
            return;
        }
    }
}

TrafficLightPhase TrafficLight::getCurrentPhase()
{
    return _currentPhase;
}

void TrafficLight::simulate()
{
    threads.emplace_back(std::thread(&TrafficLight::cycleThroughPhases, this));
}

// virtual function which is executed in a thread
void TrafficLight::cycleThroughPhases()
{
    srand(time(0));
    uint8_t cycle_duration = 4 + (rand() % 3);
    auto t1 = std::chrono::high_resolution_clock::now();
    while (true)
    {
        auto t2 = std::chrono::high_resolution_clock::now();
        auto current_duration = std::chrono::duration_cast<std::chrono::seconds>(t2 - t1).count();
        if (current_duration >= cycle_duration)
        {
            _currentPhase = (_currentPhase == TrafficLightPhase::red) ? TrafficLightPhase::green : TrafficLightPhase::red;
            _messageQueue.send(std::move(getCurrentPhase()));
            t1 = std::chrono::high_resolution_clock::now();
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
}