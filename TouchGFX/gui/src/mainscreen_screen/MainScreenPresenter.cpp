#include <gui/mainscreen_screen/MainScreenView.hpp>
#include <gui/mainscreen_screen/MainScreenPresenter.hpp>

MainScreenPresenter::MainScreenPresenter(MainScreenView& v)
    : view(v)
{

}

void MainScreenPresenter::activate()
{

}

void MainScreenPresenter::deactivate()
{

}
void MainScreenPresenter::gotoGameOverScreen()
{
    static_cast<FrontendApplication*>(Application::getInstance())->gotoGameOverScreenScreenSlideTransitionEast();
}