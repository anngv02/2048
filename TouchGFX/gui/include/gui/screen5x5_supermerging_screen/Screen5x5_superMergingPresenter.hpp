#ifndef SCREEN5X5_SUPERMERGINGPRESENTER_HPP
#define SCREEN5X5_SUPERMERGINGPRESENTER_HPP

#include <gui/model/ModelListener.hpp>
#include <mvp/Presenter.hpp>

using namespace touchgfx;

class Screen5x5_superMergingView;

class Screen5x5_superMergingPresenter : public touchgfx::Presenter, public ModelListener
{
public:
    Screen5x5_superMergingPresenter(Screen5x5_superMergingView& v);

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

    virtual ~Screen5x5_superMergingPresenter() {}

private:
    Screen5x5_superMergingPresenter();

    Screen5x5_superMergingView& view;
};

#endif // SCREEN5X5_SUPERMERGINGPRESENTER_HPP
