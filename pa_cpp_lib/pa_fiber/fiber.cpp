#include "fiber.h"
#include "../pa_utils/boost_context.h"


namespace pa_fiber{
    const int one_processor_q_size=500;
    Option<FiberGlobalCtx> _fiber_global_ctx;
    thread_local FiberThreadLocalCtx _fiber_thread_local_ctx;

    void FiberGlobalCtx::set_thread_num(int num){
        processors_.clear();
        processor_recv_qs_.clear();
        for (size_t i = 0; i < num; i++)
        {
            processor_recv_qs_.emplace_back(one_processor_q_size);
            auto &q=processor_recv_qs_.back();
            processors_.emplace_back([this,&q](){
                _fiber_thread_local_ctx.reset_and_init();
                //本身也得是一个fiber，才能切换回来
                for(;;){
                    Fiber fibers[20];
                    auto cnt=q.wait_dequeue_bulk(fibers,20);
                    for (size_t i = 0; i < cnt; i++)
                    {
                        auto &f=fibers[i];
                        //1.执行fiber

                        //2 判断fiber状态
                        switch (f.cur_status())
                        {
                        case FiberStatus::Pending:
                            
                            break;
                        case FiberStatus::Done:

                            break;
                        }
                    }
                }           
            });
        }
    }
    void Fiber::pend(){

    }
    void Fiber::start_or_continue(){
        
    }
    static void jump_in(BoostContextFrom from){
        //1.第一次jump in，需要记录下proccesor ctx
        _fiber_thread_local_ctx.
    }
    Fiber& Fiber::reset_and_init(void(*fn)()){
        this->mem_stack_.clear();
        this->mem_stack_.resize(FiberStackSize);
        this->fn_=fn;
        make_fcontext(this->mem_stack_.data(),mem_stack_.size(),Fiber::jump_in);
        return *this;
    }

    
    // FiberThreadLocalCtx
    void FiberThreadLocalCtx::reset_and_init(){
        processor_ctx=nullptr;
    }

    //FiberRuntimeBuilder
    void FiberRuntimeBuilder::set_thread_num(int num){
        building_ctx_.ref()
    }
    void FiberRuntimeBuilder::start_with_one(std::function<void()> task){
        _fiber_global_ctx=std::move(building_ctx_);
    }
    
   
}