#ifndef SCREEN3X3PRESENTER_HPP
#define SCREEN3X3PRESENTER_HPP

#include <gui/model/ModelListener.hpp>
#include <mvp/Presenter.hpp>

using namespace touchgfx;

class Screen3x3View;

class Screen3x3Presenter : public touchgfx::Presenter, public ModelListener
{
public:
    Screen3x3Presenter(Screen3x3View& v);

    /**
     * The activate function is called automatically when this screen is "switched in"
     * (ie. made active). Initialization logic can be placed here.
     */
    virtual void activate();

    /**
     * The deactivate function is called automatically when this screen is "switched out"
     * (ie. made inactive). Teardown functionality can be placed here.
     */
    virtual void deactivate();

    virtual ~Screen3x3Presenter() {}

private:
    Screen3x3Presenter();

    Screen3x3View& view;
};

#endif // SCREEN3X3PRESENTER_HPP
