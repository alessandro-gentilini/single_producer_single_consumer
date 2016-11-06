#include <thread>
#include <atomic>
#include <sstream>

#include <boost/lockfree/spsc_queue.hpp>

#include <iostream>


int producer_count = 0;
std::atomic_int consumer_count (0);

boost::lockfree::spsc_queue<std::string, boost::lockfree::capacity<16> > spsc_queue;

const int iterations = 128;

void producer(void)
{
    for (int i = 0; i != iterations; ++i) {
        int value = ++producer_count;
        std::ostringstream oss;
        oss << value;
        while (!spsc_queue.push(oss.str()))
            ;
    }
    std::cout << "Ciao povery! Io ho finito!\n" << std::flush;
}

std::atomic<bool> done (false);

void consumer(void)
{
    using namespace std::chrono_literals;
    std::string value;
    while (!done) {
        while (spsc_queue.pop(value)){
            std::cout << value << " " << std::flush;
            std::this_thread::sleep_for(100ms);
            ++consumer_count;
        }
    }

    while (spsc_queue.pop(value)){
        std::cout << value << " " << std::flush;
        std::this_thread::sleep_for(100ms);
        ++consumer_count;
    }
    std::cout << "\n";
}




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
