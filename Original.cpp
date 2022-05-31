#include <iostream>
#include <coroutine>
#include <random>
#include <vector>

using namespace std;
using viter = vector<int>::iterator;

struct mypair {
    viter first, second;
};

class Task {
public:
    // 규칙 1. C++에서 정의된 규칙을 구현한 promise_type 이라는 이름의 타입이 정의되어야 한다.
    struct promise_type {
        mypair value;

        // 사용자 정의 "코루틴 반환 객체"를 반환 한다
        Task get_return_object() {
            return Task{coroutine_handle<promise_type>::from_promise(*this)};
        }

        // 코루틴 최초 실행 시 호출. awaitable 객체를 반환 한다.
        auto initial_suspend() { return suspend_always{}; }

        auto yield_value(mypair a) {
            value = a;
            return suspend_always{};
        }

        // co_return을 사용하는 경우 구현. 나중에 코루틴 종료를 설명 할 때 같이 설명
        auto return_void() { return suspend_never{}; }

        // 코루틴 종료 시 호출. 나중에 코루틴 종료를 설명 할 때 같이 설명
        auto final_suspend() const noexcept { return suspend_always{}; }

        // 코루틴 수행 중 예외 발생 시 호출
        void unhandled_exception() { exit(7777); }
    };

    // 규칙 2. coroutine_handle<promise_type> 타입의 멤버 변수가 있어야 한다.
    coroutine_handle<promise_type> co_handler;

    // 규칙 3. coroutine_handle<promise_type> 을 인자로 받아
    // 멤버 변수를 초기화 하는 생성자가 있어야 한다.
    Task(coroutine_handle<promise_type> handler) : co_handler(handler) {};

    // 규칙 4. 소멸자에서 coroutine_handle<promise_type> 타입의
    // 코루틴 핸들러 멤버 변수의 destroy를 호출 해야 한다.
    ~Task() {
        if ((bool) co_handler) {
            co_handler.destroy();
        }
    }
};

Task sorting(viter pivot, viter left, viter right, viter* result){
    while (true){
        while(left<=right && *left<*pivot) left++;
        while(left<=right && *right>=*pivot) right--;
        if(right<left){
            swap(*right,*pivot);
            co_yield {right,pivot};
            *result = right;
            break;
        }
        swap(*right, *left);
        co_yield {right,left};
    }
}

Task QuickSort(viter start, viter end){
    end--;
    if(start>=end){
        co_return;
    }
    viter pivot;
    Task tempTask1 = sorting(start,start+1,end,&pivot);
    while (true){
        tempTask1.co_handler.resume();
        if(tempTask1.co_handler.done()) break;
        co_yield tempTask1.co_handler.promise().value;
    }
    Task tempTask2 = QuickSort(start,pivot);
    while (true){
        tempTask2.co_handler.resume();
        if(tempTask2.co_handler.done()) break;
        co_yield tempTask2.co_handler.promise().value;
    }
    Task tempTask3 = QuickSort(pivot+1,++end);
    while (true){
        tempTask3.co_handler.resume();
        if(tempTask3.co_handler.done()) break;
        co_yield tempTask3.co_handler.promise().value;
    }
}

Task SelectionSort(viter start, viter end) {
    viter minPointer;
    for (auto i = start; i != end - 1; i++) {
        minPointer = i;
        for (auto j = i + 1; j != end; j++) {
            if (*minPointer > *j) {
                co_yield {minPointer,j};
                minPointer = j;
            }
        }
        swap(*i, *minPointer);
        co_yield {i, minPointer};
    }
}

Task BubbleSort(viter start, viter end) {
    for (viter i = end - 1; i != start; i--) {
        for (viter j = start; j != i; j++) {
            if (*j > *(j + 1)) {
                swap(*j, *(j + 1));
                co_yield {j, j + 1};
            }
        }
    }
}

std::random_device rd;
std::mt19937 gen(rd());
template<typename RandomSequence>
inline void Randomization(RandomSequence &nums) {
    std::vector<bool> overlap(nums.size(), false);
    std::uniform_int_distribution<int> uni(1, nums.size());
    for (auto &x: nums) {
        int v = uni(gen);
        while (overlap[v - 1]) {
            v = uni(gen);
        }
        overlap[v - 1] = true;
        x = v;
    }
}

class mainBoard {
    vector<int> nums;
    Task* curTask;

public:
    mainBoard(int s){
        nums.resize(s);
        Randomization(nums);
        curTask = new Task(SelectionSort(nums.begin(),nums.end()));
    }

    void start(int mode){
        delete curTask;
        switch (mode) {
            case 1:
                curTask = new Task(SelectionSort(nums.begin(),nums.end()));
                cout << "선택 정렬\n";
                break;
            case 2:
                curTask = new Task(BubbleSort(nums.begin(),nums.end()));
                cout << "버블 정렬\n";
                break;
            case 3:
                curTask = new Task(QuickSort(nums.begin(),nums.end()));
                cout << "퀵 정렬\n";
                break;
        }
    }

    void reset(){
        Randomization(nums);
    }

    bool mainUpdate() {
        curTask->co_handler.resume();
        if (curTask->co_handler.done()) return false;
        return true;
    }
    int get1() {return *curTask->co_handler.promise().value.first;}
    int get2() {return *curTask->co_handler.promise().value.second;}
    void printer(){
        for(int x: nums){
            cout << x << ' ';
        }
        cout << '\n';
    }
};

int main() {
    int input;
    cout << "배열 크기 입력: ";
    cin >> input;
    mainBoard board(input);
    while(true){
        cout << "원하는 정렬을 선택하세요: " << '\n';
        cout << "선택 정렬: 1, 버블 정렬 2, 퀵 정렬 3, 종료 4" << '\n';
        cin >> input;
        if(input == 4) break;
        board.start(input);
        cout << "start coroutine\n";
        int idx=1;
        while (board.mainUpdate()){
            cout << "연산 횟수! " << idx << '\n';
            cout << "바뀐 두 항!: " << board.get1() << ' ' << board.get2() << '\n';
            idx++;
        }
        cout << "최종 결과!\n";
        board.printer();
        board.reset();
    }
    return 0;
}
