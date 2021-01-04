#ifndef TRAFFICLIGHT_H
#define TRAFFICLIGHT_H

#include <mutex>
#include <deque>
#include <condition_variable>
#include "TrafficObject.h"

// forward declarations to avoid include cycle
class Vehicle;

template <class T>
class MessageQueue
{
public:
    T receive();
    void send(T &&msg);

private:
    std::mutex _mutex;
    std::condition_variable _cond;
    std::deque<T> _queue;
};

// Simple enum to store traffic light states
enum TrafficLightPhase
{
    red,
    green,
};

// child class of TrafficObject to represent traffic lights at each intersection
class TrafficLight : public TrafficObject
{
public:
    // constructor
    TrafficLight();

    // getters / setters
    TrafficLightPhase getCurrentPhase();

    // typical behaviour methods
    void waitForGreen();
    void simulate();

private:
    // private method to switch state of traffic light 
    void cycleThroughPhases();

    // declare private MessageQueue for TrafficLightPhase objects
    MessageQueue<TrafficLightPhase> _phaseQueue;
    
    // private variable to store state of traffic light
    TrafficLightPhase _currentPhase;
    std::condition_variable _condition;
    std::mutex _mutex;
};

#endif