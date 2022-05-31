#include <iostream>

#include "GameBoard.h"
#include "CoolTimeManager.h"

const int windowWidth = 1500, windowHeight = 800;

Event e;

int main(){   
    Clock clock;
    GameBoard* Board = new GameBoard(windowHeight);
    RenderWindow *window = new RenderWindow(VideoMode(windowWidth, windowHeight), "Sorting Animation!");
    window->setFramerateLimit(144);

    std::cout << "1: ���� ����, 2: ���� ����, 3: �� ���� (����Ű)\n";
    std::cout << "R: ���� ���� �� �迭 ������ȭ\n";
    std::cout << "E: ���� ������Ʈ ������ ����\n";

    while (window->isOpen()){
        while (window->pollEvent(e)){
            if (e.type == Event::Closed) {
                window->close();
            }
        }
        window->clear();

        if (Keyboard::isKeyPressed(Keyboard::R)
            && CoolTimeMgr::ResetCoolDown(clock.getElapsedTime().asMilliseconds())) {
            Board->curState = GameBoard::DoNothing;
            Board->Reset();
        }
        if (Keyboard::isKeyPressed(Keyboard::Num1) 
            && Board->curState == GameBoard::DoNothing) {
            Board->StartSorting(SortingMode::SelectionSort);
        }
        if (Keyboard::isKeyPressed(Keyboard::Num2)
            && Board->curState == GameBoard::DoNothing) {
            Board->StartSorting(SortingMode::BubbleSort);
        }
        if (Keyboard::isKeyPressed(Keyboard::Num3)
            && Board->curState == GameBoard::DoNothing) {
            Board->StartSorting(SortingMode::QuickSort);
        }
        if (Keyboard::isKeyPressed(Keyboard::E)
            && CoolTimeMgr::ButtonCoolDown(clock.getElapsedTime().asMilliseconds())) {
            int delay;
            again:
            std::cout << "���� ������Ʈ ������ ���� (Millisecond): ";
            std::cin >> delay;
            if (delay < 0) goto again;
            std::cout << "������ " << delay << " Milliseconds�� ������.\n";
            CoolTimeMgr::setUpdateCool(delay);
        }
        
        if (CoolTimeMgr::UpdateCoolDown(clock.getElapsedTime().asMilliseconds())) {
            Board->MainUpdate();
        }

        window->draw(*Board);
        window->display();
    }

    delete window; delete Board;
    return 0;
}
