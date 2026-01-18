#include <gui/gameoverscreen_screen/GameOverScreenView.hpp>
#include <gui/common/GameGlobal.hpp>

// ==============================================================================
// LƯU Ý: View KHÔNG truy cập GPIO trực tiếp - tuân thủ MVP pattern
// ==============================================================================

GameOverScreenView::GameOverScreenView()
{
}

void GameOverScreenView::setupScreen()
{   
    // Gán dữ liệu vào các TextArea buffer
    // yourScore: điểm của ván vừa chơi
    // getCurrentBestScore(): lấy best score của game mode vừa chơi
    Unicode::snprintf(scoreTextBuffer, SCORETEXT_SIZE, "%u", GameGlobal::yourScore);
    Unicode::snprintf(bestTextBuffer, BESTTEXT_SIZE, "%u", GameGlobal::getCurrentBestScore());

    scoreText.invalidate();
    bestText.invalidate();

    GameOverScreenViewBase::setupScreen();
}

void GameOverScreenView::tearDownScreen()
{
    GameOverScreenViewBase::tearDownScreen();
}

/**
 * @brief handleTickEvent - GPIO polling đã được chuyển sang Model
 */
void GameOverScreenView::handleTickEvent()
{
    // GPIO polling được xử lý trong Model::tick()
    // View nhận events thông qua Presenter (MVP pattern)
}

// ==============================================================================
// PUBLIC METHOD - ĐƯỢC GỌI TỪ PRESENTER (MVP PATTERN)
// ==============================================================================

void GameOverScreenView::onNavigateBack()
{
    application().gotoChosing_modeScreenBlockTransition();
}
