#include <thread>
#include <atomic>

#include <boost/lockfree/spsc_queue.hpp>

class A
{
public:
    A(){}
    A(int){}
};


int producer_count = 0;
std::atomic_int consumer_count (0);

boost::lockfree::spsc_queue<A, boost::lockfree::capacity<1024> > spsc_queue;

const int iterations = 10000000;

void producer(void)
{
    for (int i = 0; i != iterations; ++i) {
        int value = ++producer_count;
        while (!spsc_queue.push(A(value)))
            ;
    }
}

std::atomic<bool> done (false);

void consumer(void)
{
    A value;
    while (!done) {
        while (spsc_queue.pop(value))
            ++consumer_count;
    }

    while (spsc_queue.pop(value))
        ++consumer_count;
}


#include <iostream>

int main(int argc, char* argv[])
{
    std::cout << "boost::lockfree::queue is ";
    if (!spsc_queue.is_lock_free())
        std::cout << "not ";
    std::cout << "lockfree\n";

    std::thread producer_thread(producer);
    std::thread consumer_thread(consumer);

    producer_thread.join();
    done = true;
    consumer_thread.join();

    std::cout << "produced " << producer_count << " objects.\n";
    std::cout << "consumed " << consumer_count << " objects.\n";
}
