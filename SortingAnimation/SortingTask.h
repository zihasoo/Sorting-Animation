#pragma once
#include <coroutine>
#include <vector>
#include <utility>

using pr = std::pair<int, int>;
using vec = std::vector<int>;

class SortingTask {
public:
    // ��Ģ 1. C++���� ���ǵ� ��Ģ�� ������ promise_type �̶�� �̸��� Ÿ���� ���ǵǾ�� �Ѵ�.
    struct promise_type {
        pr value;

        SortingTask get_return_object(); // ����� ���� "�ڷ�ƾ ��ȯ ��ü"�� ��ȯ �Ѵ�
        std::suspend_always initial_suspend(); // �ڷ�ƾ ���� ���� �� ȣ��. awaitable ��ü�� ��ȯ �Ѵ�.
        std::suspend_always yield_value(pr a); //co_yield ���� co_await�� �� �Լ��� ȣ���.
        std::suspend_always final_suspend() const noexcept;// �ڷ�ƾ ���� �� ȣ��. 
        void return_void(); // co_return ���� ȣ���.
        void unhandled_exception(); // �ڷ�ƾ ���� �� ���� �߻� �� ȣ��
    };

    // ��Ģ 2. coroutine_handle<promise_type> Ÿ���� ��� ������ �־�� �Ѵ�.
    std::coroutine_handle<promise_type> co_handler;

    // ��Ģ 3. coroutine_handle<promise_type> �� ���ڷ� �޾�
    // ��� ������ �ʱ�ȭ �ϴ� �����ڰ� �־�� �Ѵ�.
    SortingTask(std::coroutine_handle<promise_type> handler);

    // ��Ģ 4. �Ҹ��ڿ��� coroutine_handle<promise_type> Ÿ����
    // �ڷ�ƾ �ڵ鷯 ��� ������ destroy�� ȣ�� �ؾ� �Ѵ�.
    ~SortingTask();
};