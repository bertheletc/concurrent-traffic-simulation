#include <iostream>
#include <random>
#include <chrono>
#include "TrafficLight.h"

/* Implementation of class "MessageQueue" */

template <typename T>
T MessageQueue<T>::receive()
{
    // perform queue modification under the lock
    std::unique_lock<std::mutex> uLock(_mutex);
    _cond.wait(uLock, [this] { return !_queue.empty(); }); // pass unique lock to condition variable

    // remove last vector element from queue
    T msg = std::move(_queue.back());
    //_queue.pop_back();
    _queue.clear();

    return msg;
}

template <typename T>
void MessageQueue<T>::send(T &&msg)
{
    // perform vector modification under the lock
    std::lock_guard<std::mutex> uLock(_mutex);
    // add vector to queue
    _queue.push_back(std::move(msg));
    // notify client after pushing new Vehicle into vector
    _cond.notify_one();
}

/* Implementation of class "TrafficLight" */

TrafficLight::TrafficLight()
{
    _currentPhase = TrafficLightPhase::red;
}

void TrafficLight::waitForGreen()
{
    while (true)
    {
        // repeatedly call message queue receive method
        TrafficLightPhase phase = _phaseQueue.receive();
        
        if (phase == TrafficLightPhase::green)
        {
            // once a green light is received, return from the method
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
    // launch traffic light phase cycling in a thread
    threads.emplace_back(std::thread(&TrafficLight::cycleThroughPhases, this));
}

void TrafficLight::cycleThroughPhases()
{
    // Set up random number generation similar to how its done in Vehicle.cpp
    std::random_device rd;
    std::mt19937 eng(rd());
    // Distribution to represent time between 4 and 6 seconds in ms
    std::uniform_int_distribution<> distr(4000, 6000);
    
    // Calculate first time interval
    unsigned int intervalTime = distr(eng);
    // Initialize the stop watch for the first time outside of the infinite loop 
    auto stopWatchStart = std::chrono::high_resolution_clock::now();

    // Begin infinite loop to cycle traffic lights between red and green states
    while (true) 
    {
        // Sleep for 1 ms to prevent over stressing the CPU
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        // Get time since last update using duration method in chrono library
        unsigned int timeInterval = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - stopWatchStart).count();

        if (timeInterval >= intervalTime) 
        {
            if (_currentPhase == TrafficLightPhase::red) 
            {
                // switch state from red to green
                _currentPhase = TrafficLightPhase::green;
            }
            else 
            {
                // switch state from green to red
                _currentPhase = TrafficLightPhase::red; 
            }
            // Send update message to queue
            _phaseQueue.send(std::move(_currentPhase));
            // Calculate next time interval
            intervalTime = distr(eng);
            // Reset stopwatch
            stopWatchStart = std::chrono::high_resolution_clock::now();
        } 
    }
}