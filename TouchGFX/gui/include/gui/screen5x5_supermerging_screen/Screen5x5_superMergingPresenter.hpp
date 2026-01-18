#ifndef SCREEN5X5_SUPERMERGINGPRESENTER_HPP
#define SCREEN5X5_SUPERMERGINGPRESENTER_HPP

#include <gui/model/ModelListener.hpp>
#include <mvp/Presenter.hpp>

using namespace touchgfx;

class Screen5x5_superMergingView;

/**
 * @class Screen5x5_superMergingPresenter
 * @brief Presenter trong kiến trúc MVP cho Screen5x5_superMerging
 */
class Screen5x5_superMergingPresenter : public touchgfx::Presenter, public ModelListener
{
public:
    Screen5x5_superMergingPresenter(Screen5x5_superMergingView& v);

    virtual void activate();
    virtual void deactivate();

    virtual ~Screen5x5_superMergingPresenter() {}

    // ==============================================================================
    // Override ModelListener virtual functions
    // ==============================================================================
    virtual void onButtonUp() override;
    virtual void onButtonDown() override;
    virtual void onButtonLeft() override;
    virtual void onButtonRight() override;
    virtual void onButtonBack() override;

private:
    Screen5x5_superMergingPresenter();

    Screen5x5_superMergingView& view;
};

#endif // SCREEN5X5_SUPERMERGINGPRESENTER_HPP
