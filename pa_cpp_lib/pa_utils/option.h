#pragma once
#include "string.h"
namespace pa_utils{
    
    template<typename T>
    struct Option{
        Option(bool is_null){
            is_null_=is_null;
            if(!is_null){
                new (data_) T();
            }
        }
        Option(Option const&) = delete;
		Option& operator=(Option const&) = delete;
        Option(Option && movein){
            //销毁原来的
            if(!is_null()){
                T v=std::move(*(T*)data_);
            }
            memcpy(this,&movein,sizeof(Option<T>));
        } 
        Option& operator=(Option  && movein){
            //销毁原来的
            if(!is_null()){
                T v=std::move(*(T*)data_);
            }
            memcpy(this,&movein,sizeof(Option<T>));
            movein.is_null_=true;
            return *this;
        } 
        bool is_null(){
            return is_null_;
        }
        T& ref(){
            return *(T*)data_;
        }
    private:
        bool is_null_;
        char data_[sizeof(T)];
    };
}