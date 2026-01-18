#include <gui/screen5x5_lettermerging_screen/Screen5x5_letterMergingView.hpp>
#include <gui/screen5x5_lettermerging_screen/Screen5x5_letterMergingPresenter.hpp>

Screen5x5_letterMergingPresenter::Screen5x5_letterMergingPresenter(Screen5x5_letterMergingView& v)
    : view(v)
{
}

void Screen5x5_letterMergingPresenter::activate()
{
}

void Screen5x5_letterMergingPresenter::deactivate()
{
}

// ==============================================================================
// Button Event Handlers - Nhận từ Model, forward đến View
// ==============================================================================

void Screen5x5_letterMergingPresenter::onButtonUp()
{
    view.onMoveUp();
}

void Screen5x5_letterMergingPresenter::onButtonDown()
{
    view.onMoveDown();
}

void Screen5x5_letterMergingPresenter::onButtonLeft()
{
    view.onMoveLeft();
}

void Screen5x5_letterMergingPresenter::onButtonRight()
{
    view.onMoveRight();
}

void Screen5x5_letterMergingPresenter::onButtonBack()
{
    view.onNavigateBack();
}
