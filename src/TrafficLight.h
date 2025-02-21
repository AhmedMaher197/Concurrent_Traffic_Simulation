#ifndef TRAFFICLIGHT_H
#define TRAFFICLIGHT_H

#include <mutex>
#include <deque>
#include <condition_variable>
#include "TrafficObject.h"

// forward declarations to avoid include cycle
class Vehicle;

enum class TrafficLightPhase : uint8_t
{
    red = 0,
    green
};

template <class T>
class MessageQueue
{
public:
    void send(T&&);
    T receive();
private:
    std::deque<T> _queue;
    std::condition_variable _condition;
    std::mutex _mtx;
};

class TrafficLight : public TrafficObject
{
public:
    // constructor / desctructor
    TrafficLight();
    ~TrafficLight() = default;
    // getters / setters

    // typical behaviour methods
    void waitForGreen();
    void simulate();
    TrafficLightPhase getCurrentPhase();
private:
    // typical behaviour methods
    void cycleThroughPhases();

    std::condition_variable _condition;
    std::mutex _mutex;
    TrafficLightPhase _currentPhase;
    MessageQueue<TrafficLightPhase> _messageQueue;
};

#endif