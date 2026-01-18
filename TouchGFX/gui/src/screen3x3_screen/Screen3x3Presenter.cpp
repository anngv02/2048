#include <gui/screen3x3_screen/Screen3x3View.hpp>
#include <gui/screen3x3_screen/Screen3x3Presenter.hpp>

Screen3x3Presenter::Screen3x3Presenter(Screen3x3View& v)
    : view(v)
{
}

void Screen3x3Presenter::activate()
{
}

void Screen3x3Presenter::deactivate()
{
}

// ==============================================================================
// Button Event Handlers - Nhận từ Model, forward đến View
// ==============================================================================

void Screen3x3Presenter::onButtonUp()
{
    view.onMoveUp();
}

void Screen3x3Presenter::onButtonDown()
{
    view.onMoveDown();
}

void Screen3x3Presenter::onButtonLeft()
{
    view.onMoveLeft();
}

void Screen3x3Presenter::onButtonRight()
{
    view.onMoveRight();
}

void Screen3x3Presenter::onButtonBack()
{
    view.onNavigateBack();
}
