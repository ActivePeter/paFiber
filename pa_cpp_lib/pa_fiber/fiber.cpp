#include "fiber.h"
#include "../pa_utils/boost_context.h"
#include "../pa_utils/error.h"

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
                        //尝试恢复到 poll看看阻塞结束没有
                        f.poll();
                        if(f.cur_status()!=FiberStatus::Pending){
                            f.start_or_continue();
                        }
                        //2 判断fiber状态
                        switch (f.cur_status())
                        {
                        case FiberStatus::Running://刚开始
                        case FiberStatus::Pending://超时或阻塞的系统调用
                            if(!q.try_enqueue(std::move(f))){
                                //本地队列满了，加到全局队列去
                                _fiber_global_ctx.ref()._add_fiber(std::move(f));
                            }
                            break;
                        case FiberStatus::Done:
                            //需要通知其他等待他的fiber
                            f.on_stop();
                            break; 
                        }
                    }
                }           
            });
        }
    }
    void FiberGlobalCtx::wait_end(){
        std::mutex mu;
        runtime_end_waiter_.wait(mu);
    }
    void FiberGlobalCtx::go(std::function<void()> fiber_fn){
        auto fiber=Fiber();
        fiber.reset_and_init(fiber_fn);
        _add_fiber(std::move(fiber));
    }
    void FiberGlobalCtx::_add_fiber(Fiber&&fiber){
        int min_len_queue_i=0;
        int min_len=processor_recv_qs_[0].size_approx();
        for (size_t i = 1; i < processor_recv_qs_.size(); i++)
        {
            auto sz=processor_recv_qs_[i].size_approx();
            if(sz<min_len){
                min_len=sz;
            }
            /* code */
        }
        if(!processor_recv_qs_[min_len_queue_i].try_enqueue(std::move(fiber))){
            if(!this->global_fiber_queue_.enqueue(std::move(fiber))){
                printf("enqueue failed");
                abort();
            }
        }
    }

    void Fiber::done(){
        this->cur_status=FiberStatus::Done;
        jump_fcontext(_fiber_thread_local_ctx.processor_ctx(),nullptr);//跳转回进入前
    }
    void Fiber::pend(){
        this->cur_status=FiberStatus::Pending;
        jump_fcontext(_fiber_thread_local_ctx.processor_ctx(),nullptr);//跳转回进入前
    }
    void Fiber::start_or_continue(){
        _fiber_thread_local_ctx.record_cur_fiber(this);
        jump_fcontext(this->fiber_ctx_,nullptr);
    }
    static void Fiber::jump_in(BoostContextFrom from){
        //1.第一次jump in，需要记录下proccesor ctx
        _fiber_thread_local_ctx.record_processor_ctx(from.fctx);
        //2.执行fiber函数
        _fiber_thread_local_ctx.cur_fiber()->into_fn();
        _fiber_thread_local_ctx.cur_fiber()->done();
    }
    Fiber& Fiber::reset_and_init(std::function<void()> fn){
        this->mem_stack_.clear();
        this->mem_stack_.resize(FiberStackSize);
        this->fn_=fn;
        fiber_ctx_=make_fcontext(this->mem_stack_.data(),mem_stack_.size(),Fiber::jump_in);
        return *this;
    }
    void Fiber::poll(){
        if(this->cur_status()!=FiberStatus::Pending){
            return;
        }
        
    }
    
    // FiberThreadLocalCtx
    void FiberThreadLocalCtx::reset_and_init(){
        processor_ctx_=nullptr;
    }

    //FiberRuntimeBuilder
    FiberRuntimeBuilder& FiberRuntimeBuilder::set_thread_num(int num){
        building_ctx_.ref().set_thread_num(num);
        return *this;
    }
    // void FiberRuntimeBuilder::start(){
    //     _fiber_global_ctx=std::move(building_ctx_);
    //     _fiber_global_ctx.ref().start();
    // }
    void FiberRuntimeBuilder::start_with_fiber(std::function<void()> fiber){
        _fiber_global_ctx=std::move(building_ctx_);
        _fiber_global_ctx.ref().start();
        go(fiber);
        _fiber_global_ctx.ref().wait_end();
    }  
    // struct {
    void FiberHandle::wait(){
        //1.被wait的fiber做终止后标记
        this->handle_->_set_waited_by(_fiber_thread_local_ctx.cur_fiber());
        //2.当前fiber 进入pending模式
        _fiber_thread_local_ctx.cur_fiber()->pend();
    }
    // }
    FiberHandle go(std::function<void()> task){
        
        _fiber_global_ctx.ref().
    }
}