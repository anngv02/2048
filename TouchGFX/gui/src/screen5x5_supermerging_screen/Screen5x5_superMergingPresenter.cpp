#include <gui/screen5x5_supermerging_screen/Screen5x5_superMergingView.hpp>
#include <gui/screen5x5_supermerging_screen/Screen5x5_superMergingPresenter.hpp>

Screen5x5_superMergingPresenter::Screen5x5_superMergingPresenter(Screen5x5_superMergingView& v)
    : view(v)
{
}

void Screen5x5_superMergingPresenter::activate()
{
}

void Screen5x5_superMergingPresenter::deactivate()
{
}

// ==============================================================================
// Button Event Handlers - Nhận từ Model, forward đến View
// ==============================================================================

void Screen5x5_superMergingPresenter::onButtonUp()
{
    view.onMoveUp();
}

void Screen5x5_superMergingPresenter::onButtonDown()
{
    view.onMoveDown();
}

void Screen5x5_superMergingPresenter::onButtonLeft()
{
    view.onMoveLeft();
}

void Screen5x5_superMergingPresenter::onButtonRight()
{
    view.onMoveRight();
}

void Screen5x5_superMergingPresenter::onButtonBack()
{
    view.onNavigateBack();
}
