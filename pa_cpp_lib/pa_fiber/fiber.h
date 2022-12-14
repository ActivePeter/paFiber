#pragma once
#include "functional"
#include "vector"
#include "thread"
#include "condition_variable"

#include "../pa_utils/sync_queue.h"
#include "../pa_utils/option.h"
#include "fiber.h"

namespace pa_fiber{
    using namespace pa_utils;
    struct Fiber;
    
    struct FiberHandle{
        void wait();
    private:
        Fiber* handle_;
    };
    FiberHandle go(std::function<void()> task);

    struct FiberRuntimeBuilder{
        FiberRuntimeBuilder& set_thread_num(int num);
        // void start();    
        void start_with_fiber(std::function<void()> fiber);    
    private:
        Option<FiberGlobalCtx> building_ctx_{false};
    };
    
    
    enum class FiberStatus{
        Running,
        Pending,//系统调用，锁，cond调用，
        Done,
    };
    struct Fiber{
        static const int FiberStackSize=8000;
        FiberStatus cur_status(){
            return status_;
        }
        void poll();
        void done();
        void pend();//让出，转到proccssor
        void stop_pending(){
            status_=FiberStatus::Running;
        }
        void start_or_continue();
        static void jump_in(BoostContextFrom);
        Fiber& reset_and_init(std::function<void()> fn);
        void into_fn(){
            if(fn_){
                fn_();
            }else{
                printf("fiber no fn \n");
                abort();
            }
        }
        void _set_waited_by(Fiber*fiber){
            being_waited_by_=fiber;
        }
        void on_stop(){
            if(being_waited_by_){
                being_waited_by_->stop_pending();
                being_waited_by_=nullptr;
            }
        }
    private:
        BoostContext fiber_ctx_;
        FiberStatus status_=FiberStatus::Running; 
        std::function<void()> fn_;
        std::vector<char> mem_stack_;
        Fiber* being_waited_by_=nullptr;//只可能被一个协程等待
    };

    


    struct FiberGlobalCtx{
        void set_thread_num(int num);
        void start();
        void wait_end();
        void go(std::function<void()> fiber_fn);
        void _add_fiber(Fiber&&fiber);
        FiberGlobalCtx(FiberGlobalCtx const&) = delete;
		FiberGlobalCtx& operator=(FiberGlobalCtx const&) = delete;

    private:
        std::condition_variable runtime_end_waiter_;
        std::vector< std::thread > processors_;
        pa_utils::BlockingConcurrentQueue<Fiber> global_fiber_queue_;//当queues都满了后，就加入到global
        std::vector<pa_utils::BlockingConcurrentQueue<Fiber>> processor_recv_qs_;
        bool stop=false;
    };

    struct FiberThreadLocalCtx{
        void reset_and_init();
        void record_processor_ctx(BoostContext processor_ctx){
            if(!processor_ctx_){
                processor_ctx_=processor_ctx;
            }
        }
        void record_cur_fiber(Fiber*fiber){
            cur_fiber_=fiber;
        }
        Fiber* cur_fiber(){
            return cur_fiber_;
        }
        BoostContext processor_ctx(){
            return processor_ctx_;
        }
    private:
        BoostContext processor_ctx_=nullptr;
        Fiber* cur_fiber_;
    };
}