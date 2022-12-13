1. fiber被生成，扔给线程池
2. 原线程应具备wait的能力，标记waiting，到wait的事件（协程执行/条件变量）结束后，将标记改回来
3. 协程锁
4. 使用boost context切换栈