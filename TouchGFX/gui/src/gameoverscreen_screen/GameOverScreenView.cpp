#include <gui/gameoverscreen_screen/GameOverScreenView.hpp>
#include <gui/common/GameGlobal.hpp>
#include <stm32f4xx_hal.h>
GameOverScreenView::GameOverScreenView()
{

}

void GameOverScreenView::setupScreen()
{   
    // Gán dữ liệu vào các TextArea buffer
    Unicode::snprintf(scoreTextBuffer, SCORETEXT_SIZE, "%u", GameGlobal::yourScore);
    Unicode::snprintf(bestTextBuffer, BESTTEXT_SIZE, "%u", GameGlobal::bestScore);
    scoreText.invalidate();
    bestText.invalidate();
    GameOverScreenViewBase::setupScreen();
}

void GameOverScreenView::tearDownScreen()
{
    GameOverScreenViewBase::tearDownScreen();
}
void GameOverScreenView::handleTickEvent()
{
    static uint32_t lastPressTime = 0;
    const uint32_t debounceDelay = 200; // Thời gian chống dội (ms)
    uint32_t currentTime = HAL_GetTick();

    uint8_t currentState = 0;
    currentState |= (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_0) == GPIO_PIN_SET ? 1 : 0);  // PA0: BACK

    static uint8_t lastState = 0;

    if (currentTime - lastPressTime > debounceDelay)
    {
        if ((currentState & 1) && !(lastState & 1)) // PA0: BACK
        {
            application().gotoChosing_modeScreenBlockTransition();
            lastPressTime = currentTime;
        }

        lastState = currentState;
    }
}
