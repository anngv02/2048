#ifndef CHOSING_MODEPRESENTER_HPP
#define CHOSING_MODEPRESENTER_HPP

#include <gui/model/ModelListener.hpp>
#include <mvp/Presenter.hpp>

using namespace touchgfx;

class Chosing_modeView;

class Chosing_modePresenter : public touchgfx::Presenter, public ModelListener
{
public:
    Chosing_modePresenter(Chosing_modeView& v);

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

    virtual ~Chosing_modePresenter() {}

private:
    Chosing_modePresenter();

    Chosing_modeView& view;
};

#endif // CHOSING_MODEPRESENTER_HPP
