#pragma once
#include <SFML/Graphics.hpp>
#include "SortingTask.h"

using namespace sf;

enum class SortingMode {
    SelectionSort,
    BubbleSort,
    QuickSort
};

class GameBoard : public Drawable, public Transformable {
    int RodCount = 150, RodWidth = 10, MaxRodHeight;
    SortingTask* curTask;
    VertexArray ImageBoard;
    vec MainBoard;
    pr ChangedIdx;

private:
    virtual void draw(RenderTarget&, RenderStates) const;

public:
    enum {
        Randomizationing,
        Sorting,
        DoNothing
    };
    int curState = DoNothing;
    GameBoard(int);
    void MainUpdate();
    pr UpdateImageBoard(pr);
    void RevertImageBoard(pr);
    void StartSorting(SortingMode);
    void Reset();
};
