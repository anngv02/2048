#ifndef SELECTEDGAMEDESIGNPRESENTER_HPP
#define SELECTEDGAMEDESIGNPRESENTER_HPP

#include <gui/model/ModelListener.hpp>
#include <mvp/Presenter.hpp>

using namespace touchgfx;

class SelectedGameDesignView;

class SelectedGameDesignPresenter : public touchgfx::Presenter, public ModelListener
{
public:
    SelectedGameDesignPresenter(SelectedGameDesignView& v);

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

    virtual ~SelectedGameDesignPresenter() {}

private:
    SelectedGameDesignPresenter();

    SelectedGameDesignView& view;
};

#endif // SELECTEDGAMEDESIGNPRESENTER_HPP
