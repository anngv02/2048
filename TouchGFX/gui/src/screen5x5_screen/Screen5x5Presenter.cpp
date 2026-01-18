#include <gui/screen5x5_screen/Screen5x5View.hpp>
#include <gui/screen5x5_screen/Screen5x5Presenter.hpp>

Screen5x5Presenter::Screen5x5Presenter(Screen5x5View& v)
    : view(v)
{
}

void Screen5x5Presenter::activate()
{
}

void Screen5x5Presenter::deactivate()
{
}

// ==============================================================================
// Button Event Handlers - Nhận từ Model, forward đến View
// ==============================================================================

void Screen5x5Presenter::onButtonUp()
{
    view.onMoveUp();
}

void Screen5x5Presenter::onButtonDown()
{
    view.onMoveDown();
}

void Screen5x5Presenter::onButtonLeft()
{
    view.onMoveLeft();
}

void Screen5x5Presenter::onButtonRight()
{
    view.onMoveRight();
}

void Screen5x5Presenter::onButtonBack()
{
    view.onNavigateBack();
}
