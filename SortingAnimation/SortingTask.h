#pragma once
#include <coroutine>
#include <vector>
#include <utility>

using pr = std::pair<int, int>;
using vec = std::vector<int>;

class SortingTask {
public:
    // 규칙 1. C++에서 정의된 규칙을 구현한 promise_type 이라는 이름의 타입이 정의되어야 한다.
    struct promise_type {
        pr value;

        SortingTask get_return_object(); // 사용자 정의 "코루틴 반환 객체"를 반환 한다
        std::suspend_always initial_suspend(); // 코루틴 최초 실행 시 호출. awaitable 객체를 반환 한다.
        std::suspend_always yield_value(pr a); //co_yield 사용시 co_await과 이 함수가 호출됨.
        std::suspend_always final_suspend() const noexcept;// 코루틴 종료 시 호출. 
        void return_void(); // co_return 사용시 호출됨.
        void unhandled_exception(); // 코루틴 수행 중 예외 발생 시 호출
    };

    // 규칙 2. coroutine_handle<promise_type> 타입의 멤버 변수가 있어야 한다.
    std::coroutine_handle<promise_type> co_handler;

    // 규칙 3. coroutine_handle<promise_type> 을 인자로 받아
    // 멤버 변수를 초기화 하는 생성자가 있어야 한다.
    SortingTask(std::coroutine_handle<promise_type> handler);

    // 규칙 4. 소멸자에서 coroutine_handle<promise_type> 타입의
    // 코루틴 핸들러 멤버 변수의 destroy를 호출 해야 한다.
    ~SortingTask();
};