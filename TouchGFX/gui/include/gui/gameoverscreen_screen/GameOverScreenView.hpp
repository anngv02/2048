#ifndef GAMEOVERSCREENVIEW_HPP
#define GAMEOVERSCREENVIEW_HPP

#include <gui_generated/gameoverscreen_screen/GameOverScreenViewBase.hpp>
#include <gui/gameoverscreen_screen/GameOverScreenPresenter.hpp>

/**
 * @class GameOverScreenView
 * @brief View cho màn hình Game Over
 */
class GameOverScreenView : public GameOverScreenViewBase
{
public:
    GameOverScreenView();
    virtual ~GameOverScreenView() {}
    virtual void setupScreen();
    virtual void tearDownScreen();
    virtual void handleTickEvent();

    // ==============================================================================
    // Public method để Presenter gọi (MVP pattern)
    // ==============================================================================
    void onNavigateBack();

protected:
};

#endif // GAMEOVERSCREENVIEW_HPP
