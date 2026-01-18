#ifndef SCREEN5X5_LETTERMERGINGPRESENTER_HPP
#define SCREEN5X5_LETTERMERGINGPRESENTER_HPP

#include <gui/model/ModelListener.hpp>
#include <mvp/Presenter.hpp>

using namespace touchgfx;

class Screen5x5_letterMergingView;

/**
 * @class Screen5x5_letterMergingPresenter
 * @brief Presenter trong kiến trúc MVP cho Screen5x5_letterMerging
 */
class Screen5x5_letterMergingPresenter : public touchgfx::Presenter, public ModelListener
{
public:
    Screen5x5_letterMergingPresenter(Screen5x5_letterMergingView& v);

    virtual void activate();
    virtual void deactivate();

    virtual ~Screen5x5_letterMergingPresenter() {}

    // ==============================================================================
    // Override ModelListener virtual functions
    // ==============================================================================
    virtual void onButtonUp() override;
    virtual void onButtonDown() override;
    virtual void onButtonLeft() override;
    virtual void onButtonRight() override;
    virtual void onButtonBack() override;

private:
    Screen5x5_letterMergingPresenter();

    Screen5x5_letterMergingView& view;
};

#endif // SCREEN5X5_LETTERMERGINGPRESENTER_HPP
