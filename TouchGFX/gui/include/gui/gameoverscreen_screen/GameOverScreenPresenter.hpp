#ifndef GAMEOVERSCREENPRESENTER_HPP
#define GAMEOVERSCREENPRESENTER_HPP

#include <gui/model/ModelListener.hpp>
#include <mvp/Presenter.hpp>

using namespace touchgfx;

class GameOverScreenView;

/**
 * @class GameOverScreenPresenter
 * @brief Presenter cho màn hình Game Over
 */
class GameOverScreenPresenter : public touchgfx::Presenter, public ModelListener
{
public:
    GameOverScreenPresenter(GameOverScreenView& v);

    virtual void activate();
    virtual void deactivate();

    virtual ~GameOverScreenPresenter() {}

    // ==============================================================================
    // Override ModelListener - Chỉ cần BACK button cho màn hình này
    // ==============================================================================
    virtual void onButtonBack() override;

private:
    GameOverScreenPresenter();

    GameOverScreenView& view;
};

#endif // GAMEOVERSCREENPRESENTER_HPP
