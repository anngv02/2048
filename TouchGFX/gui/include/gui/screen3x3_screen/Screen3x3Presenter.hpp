#ifndef SCREEN3X3PRESENTER_HPP
#define SCREEN3X3PRESENTER_HPP

#include <gui/model/ModelListener.hpp>
#include <mvp/Presenter.hpp>

using namespace touchgfx;

class Screen3x3View;

/**
 * @class Screen3x3Presenter
 * @brief Presenter trong kiến trúc MVP cho Screen3x3 (Game 3x3)
 */
class Screen3x3Presenter : public touchgfx::Presenter, public ModelListener
{
public:
    Screen3x3Presenter(Screen3x3View& v);

    virtual void activate();
    virtual void deactivate();

    virtual ~Screen3x3Presenter() {}

    // ==============================================================================
    // Override ModelListener virtual functions
    // ==============================================================================
    virtual void onButtonUp() override;
    virtual void onButtonDown() override;
    virtual void onButtonLeft() override;
    virtual void onButtonRight() override;
    virtual void onButtonBack() override;

private:
    Screen3x3Presenter();

    Screen3x3View& view;
};

#endif // SCREEN3X3PRESENTER_HPP
