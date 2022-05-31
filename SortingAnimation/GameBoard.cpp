#include <random>
#include "GameBoard.h"

SortingTask sorting(vec& board, int pivot, int left, int right, int* result);
SortingTask QuickSort(vec& board, int start, int end);
SortingTask SelectionSort(vec& board, int start, int end);
SortingTask BubbleSort(vec& board, int start, int end);
inline void Randomization(vec& nums);

void GameBoard::draw(RenderTarget& target, RenderStates states) const {
    states.transform *= getTransform();
    target.draw(ImageBoard, states);
}

GameBoard::GameBoard(int height):MaxRodHeight(height) {
    int h = MaxRodHeight / RodCount;
    Vertex* quad;
    ChangedIdx = { 0,1 };
    MainBoard.resize(RodCount);
    Randomization(MainBoard);
    curTask = new SortingTask(SelectionSort(MainBoard, 0, RodCount));
    ImageBoard.setPrimitiveType(Quads);
    ImageBoard.resize(RodCount * 4);
    for (int i = 0; i < RodCount; i++) {
        quad = &ImageBoard[i * 4];
        quad[0].position = Vector2f(i * RodWidth, MaxRodHeight);
        quad[1].position = Vector2f((i + 1) * RodWidth - 1, MaxRodHeight);
        quad[2].position = Vector2f((i + 1) * RodWidth - 1, MaxRodHeight - MainBoard[i] * h);
        quad[3].position = Vector2f(i * RodWidth, MaxRodHeight - MainBoard[i] * h);
        quad[0].color = Color::White;
        quad[1].color = Color::White;
        quad[2].color = Color::White;
        quad[3].color = Color::White;
    }
}

void GameBoard::StartSorting(SortingMode mode) {
    delete curTask;
    curState = Sorting;
    switch (mode){
    case SortingMode::SelectionSort:
        curTask = new SortingTask(SelectionSort(MainBoard, 0, RodCount));
        break;
    case SortingMode::BubbleSort:
        curTask = new SortingTask(BubbleSort(MainBoard, 0, RodCount));
        break;
    case SortingMode::QuickSort:
        curTask = new SortingTask(QuickSort(MainBoard, 0, RodCount));
        break;
    }  
}

pr GameBoard::UpdateImageBoard(pr value) {
    ImageBoard[value.first * 4 + 2].position.y = MaxRodHeight - MainBoard[value.first] * (MaxRodHeight / RodCount);
    ImageBoard[value.first * 4 + 3].position.y = MaxRodHeight - MainBoard[value.first] * (MaxRodHeight / RodCount);
    ImageBoard[value.second * 4 + 2].position.y = MaxRodHeight - MainBoard[value.second] * (MaxRodHeight / RodCount);
    ImageBoard[value.second * 4 + 3].position.y = MaxRodHeight - MainBoard[value.second] * (MaxRodHeight / RodCount);
    for (int i = 0; i < 4; i++) {
        ImageBoard[value.first * 4 + i].color = Color::Red;
        ImageBoard[value.second * 4 + i].color = Color::Red;
    }

    return value;
}

void GameBoard::RevertImageBoard(pr value) {
    for (int i = 0; i < 4; i++) {
        ImageBoard[value.first * 4 + i].color = Color::White;
        ImageBoard[value.second * 4 + i].color = Color::White;
    }
}

void GameBoard::MainUpdate() {
    if (curState == Randomizationing) {
        //do randomizationing -> 랜덤화 하는거 애니메이션을 추가할지 모르겠음
        return;
    }
    else if (curState == Sorting) {
        RevertImageBoard(ChangedIdx);
        curTask->co_handler.resume();
        if (curTask->co_handler.done()) {
            curState = DoNothing;
            return;
        }
        ChangedIdx = curTask->co_handler.promise().value;
        UpdateImageBoard(ChangedIdx);
    }
    //else do nothing
}

void GameBoard::Reset() {
    RevertImageBoard(ChangedIdx);
    int h = MaxRodHeight / RodCount;
    Randomization(MainBoard);
    Vertex* quad;
    for (int i = 0; i < RodCount; i++) {
        quad = &ImageBoard[i * 4];
        quad[0].position = Vector2f(i * RodWidth, MaxRodHeight);
        quad[1].position = Vector2f((i + 1) * RodWidth - 1, MaxRodHeight);
        quad[2].position = Vector2f((i + 1) * RodWidth - 1, MaxRodHeight - MainBoard[i] * h);
        quad[3].position = Vector2f(i * RodWidth, MaxRodHeight - MainBoard[i] * h);
    }
}

template<typename T>
inline void myswap(T& a, T& b) {
    T temp = a;
    a = b;
    b = temp;
}

SortingTask sorting(vec& board, int pivot, int left, int right, int* result) {
    while (true) {
        while (left <= right && board[left] < board[pivot]) left++;
        while (left <= right && board[right] >= board[pivot]) right--;
        if (right < left) {
            myswap(board[right], board[pivot]);
            co_yield{ right,pivot };
            *result = right;
            co_return;
        }
        myswap(board[right], board[left]);
        co_yield{ right,left };
    }
}

SortingTask QuickSort(vec &board, int start, int end) {
    end--;
    if (start >= end) {
        co_return;
    }
    int pivot;
    SortingTask tempTask1 = sorting(board,start, start + 1, end, &pivot);
    while (true) {
        tempTask1.co_handler.resume();
        if (tempTask1.co_handler.done()) break;
        co_yield tempTask1.co_handler.promise().value;
    }
    SortingTask tempTask2 = QuickSort(board, start, pivot);
    while (true) {
        tempTask2.co_handler.resume();
        if (tempTask2.co_handler.done()) break;
        co_yield tempTask2.co_handler.promise().value;
    }
    SortingTask tempTask3 = QuickSort(board, pivot + 1, ++end);
    while (true) {
        tempTask3.co_handler.resume();
        if (tempTask3.co_handler.done()) break;
        co_yield tempTask3.co_handler.promise().value;
    }
}

SortingTask SelectionSort(vec& board, int start, int end) {
    int minIdx;
    for (int i = start; i < end; i++) {
        minIdx = i;
        for (int j = i + 1; j < end; j++) {
            if (board[minIdx] > board[j]) {
                co_yield{ minIdx,j };
                minIdx = j;
            }
        }
        myswap(board[i], board[minIdx]);
        co_yield{ i, minIdx };
    }
}

SortingTask BubbleSort(vec& board, int start, int end) {
    for (int i = end - 1; i > start; i--) {
        for (int j = start; j < i; j++) {
            if (board[j] > board[j + 1]) {
                myswap(board[j], board[j + 1]);
                co_yield{ j, j + 1 };
            }
        }
    }
}

inline void Randomization(vec& nums) {
    std::vector<bool> overlap(nums.size() + 1, false);
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> uni(1, nums.size());
    for (auto& x : nums) {
        int v = uni(gen);
        while (overlap[v]) {
            v = uni(gen);
        }
        overlap[v] = true;
        x = v;
    }
}