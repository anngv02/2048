#ifndef SCREEN5X5PRESENTER_HPP
#define SCREEN5X5PRESENTER_HPP

#include <gui/model/ModelListener.hpp>
#include <mvp/Presenter.hpp>

using namespace touchgfx;

class Screen5x5View;

/**
 * @class Screen5x5Presenter
 * @brief Presenter trong kiến trúc MVP cho Screen5x5 (Game 5x5)
 */
class Screen5x5Presenter : public touchgfx::Presenter, public ModelListener
{
public:
    Screen5x5Presenter(Screen5x5View& v);

    virtual void activate();
    virtual void deactivate();

    virtual ~Screen5x5Presenter() {}

    // ==============================================================================
    // Override ModelListener virtual functions
    // ==============================================================================
    virtual void onButtonUp() override;
    virtual void onButtonDown() override;
    virtual void onButtonLeft() override;
    virtual void onButtonRight() override;
    virtual void onButtonBack() override;

private:
    Screen5x5Presenter();

    Screen5x5View& view;
};

#endif // SCREEN5X5PRESENTER_HPP
