#include <iostream>
#include <random>
#include <chrono>
#include "TrafficLight.h"

/* Implementation of class "MessageQueue" */

/* 
template <typename T>
T MessageQueue<T>::receive()
{
    // FP.5a : The method receive should use std::unique_lock<std::mutex> and _condition.wait() 
    // to wait for and receive new messages and pull them from the queue using move semantics. 
    // The received object should then be returned by the receive function. 
}

template <typename T>
void MessageQueue<T>::send(T &&msg)
{
    // FP.4a : The method send should use the mechanisms std::lock_guard<std::mutex> 
    // as well as _condition.notify_one() to add a new message to the queue and afterwards send a notification.
}
*/

/* Implementation of class "TrafficLight" */


TrafficLight::TrafficLight()
{
    _currentPhase = TrafficLightPhase::red;
}

void TrafficLight::waitForGreen()
{
    // FP.5b : add the implementation of the method waitForGreen, in which an infinite while-loop 
    // runs and repeatedly calls the receive function on the message queue. 
    // Once it receives TrafficLightPhase::green, the method returns.
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
    auto start = std::chrono::high_resolution_clock::now();

    // Begin infinite loop to cycle traffic lights between red and green states
    while (true) 
    {
        // Sleep for 1 ms to prevent over stressing the CPU
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        // Get time since last update using duration method in chrono library
        unsigned int timeInterval = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - start).count();

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
            start = std::chrono::high_resolution_clock::now();
        } 
    }
}