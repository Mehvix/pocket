#ifndef POCKET_TIMER_H
#define POCKET_TIMER_H

#include <thread>
#include <chrono>
#include <functional>
#include <utility>

using namespace std;

class Timer{
    thread Thread;  // used for asynchronous code execution without blocking main thread
    bool live = false;  // running
    long callNumb = -1L;  // how many times we would like to call a function
    long repeatCount = -1L;  // number of times function has been called
    chrono::milliseconds  interval = chrono::milliseconds (0);  // interval between function calls
    function<void(void)> func = nullptr;  // function that takes nothing and returns nothing

    void sleepRun(){
        this_thread::sleep_for(interval);  //pause thread for certain time interval
        if(live) Function()(); // double parenthesis - first calls Function and second calls function that Function returns
    }

    void threadFunc(){
        if(callNumb == inf) while(live) sleepRun();
        else while(repeatCount--) sleepRun();
    }

    public:  // todo move to constants
        static const long inf = -1L;

    Timer(){};
    Timer(const function<void(void)> &f) : func (f) {}
    Timer(const function<void(void)> &f, const unsigned long &i, const long repeat = Timer::inf) : func(f), interval(chrono::milliseconds(i)), callNumb(repeat){}

    void Start(bool async = true){
        if(live) return;
        live = true;
        repeatCount = callNumb;  // set repeat to how many times we need to run
        if(async) Thread = thread(&Timer::threadFunc, this);  // if thread is not being blocked
        else this->threadFunc();
    }

    void Stop(){
        live = false;
        Thread.join();  // won't be any concurrent execution, only 1 thread
    }

    bool isLive() const {return live;}

    void setFunc(const function<void(void)> &f){func = f;}
    void setInterval(const unsigned long &i) {
        if (live){return;}
        interval = chrono::milliseconds(i);
    }
    void setRepeatCnt(const long r){
        if (live) return;
        callNumb = r;
    }

    long getCnt() const {return repeatCount;}
    long getRepeatCnt() const {return callNumb;}
    unsigned long getInterval() const {return interval.count();}

    function<void(void)> Function() const{
        return func;   // returns a function to be called right after
    }
};

#endif //POCKET_TIMER_H
