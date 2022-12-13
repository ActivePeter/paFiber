#pragma once
#include "functional"
#include "vector"
#include "thread"
#include "../pa_utils/sync_queue.h"
#include "../pa_utils/option.h"


namespace pa_fiber{
    using namespace pa_utils;
    

    struct FiberRuntimeBuilder{
        void set_thread_num(int num);
        void start_with_one(void(*fn)());        
    private:
        Option<FiberGlobalCtx> building_ctx_{false};
    };
    
    
    enum class FiberStatus{
        Pending,//系统调用，锁，cond调用，
        Done,
    };
    struct Fiber{
        static const int FiberStackSize=8000;
        FiberStatus cur_status(){
            return status_;
        }
        void pend();//让出，转到proccssor
        void start_or_continue();
        static void jump_in(BoostContextFrom);
        Fiber& reset_and_init(void(*fn)());
    private:
        bool waiting_=false;//等待某个事件结束
        FiberStatus status_; 
        void(*fn_)();//协程对应函数
        std::vector<char> mem_stack_;
    };

    struct FiberHandler{
        
    private:
        Fiber* handle_;
    };
    FiberHandler fiber_start(std::function<void()> task);


    struct FiberGlobalCtx{
        void set_thread_num(int num);
        void start();
        void wait_end();
        FiberGlobalCtx(FiberGlobalCtx const&) = delete;
		FiberGlobalCtx& operator=(FiberGlobalCtx const&) = delete;
    private:
        std::vector< std::thread > processors_;
        std::vector<pa_utils::BlockingConcurrentQueue<Fiber>> processor_recv_qs_;
        bool stop=false;
    };

    struct FiberThreadLocalCtx{
        void reset_and_init();
        void record_processor_ctx(BoostContext processor_ctx){
            processor_ctx_=processor_ctx;
        }
    private:
        BoostContext processor_ctx_=nullptr;
    };
}