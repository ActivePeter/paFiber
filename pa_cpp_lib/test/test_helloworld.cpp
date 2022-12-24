#include "../pa_fiber/fiber.h"
#include "list"
#include "atomic"

    std::atomic_int cnt(1);
    
int main(){
    pa_fiber::FiberRuntimeBuilder()
        .set_thread_num(8)
        .start_with_fiber([]{
            std::list<pa_fiber::FiberHandle> handles;
            handles.push_back(pa_fiber::go([](){
                printf("hello world %d\n",cnt.fetch_add(1));
            }));
            for (auto &handle:handles)
            {
                handle.wait();
            }
        });

    return 0;
}