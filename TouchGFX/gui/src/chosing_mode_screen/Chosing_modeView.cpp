#include <gui/chosing_mode_screen/Chosing_modeView.hpp>
#include <touchgfx/Color.hpp>
#include <stm32f4xx_hal.h>

#define MODE_COUNT 3

Chosing_modeView::Chosing_modeView()
    : Chosing_modeViewBase(),
      currentIndex(0)
{
}

void Chosing_modeView::setupScreen()
{
    Chosing_modeViewBase::setupScreen();
    currentIndex = 0;
    updateHighlight();
}

void Chosing_modeView::tearDownScreen()
{
    Chosing_modeViewBase::tearDownScreen();
}

void Chosing_modeView::moveUp()
{
    if (currentIndex > 0)
        currentIndex--;
    else
        currentIndex = MODE_COUNT - 1;

    updateHighlight();
}

void Chosing_modeView::moveDown()
{
    if (currentIndex < MODE_COUNT - 1)
        currentIndex++;
    else
        currentIndex = 0;

    updateHighlight();
}

void Chosing_modeView::selectMode()
{
    if (currentIndex == 0)
    {
        application().gotoSelectedGameDesignScreenSlideTransitionEast();
    }
    else if (currentIndex == 1)
    {
        application().gotoScreen5x5_superMergingScreenSlideTransitionEast();
    }
    else if (currentIndex == 2)
    {
        application().gotoScreen5x5_letterMergingScreenSlideTransitionEast();
    }
}

void Chosing_modeView::updateHighlight()
{
    flexButton1.setBoxWithBorderColors(
        (currentIndex == 0) ? touchgfx::Color::getColorFromRGB(255, 0, 0)
                            : touchgfx::Color::getColorFromRGB(0, 102, 153),
        touchgfx::Color::getColorFromRGB(0, 153, 204),
        touchgfx::Color::getColorFromRGB(0, 51, 102),
        touchgfx::Color::getColorFromRGB(51, 102, 153));
    flexButton1.invalidate();

    flexButton2.setBoxWithBorderColors(
        (currentIndex == 1) ? touchgfx::Color::getColorFromRGB(255, 0, 0)
                            : touchgfx::Color::getColorFromRGB(0, 102, 153),
        touchgfx::Color::getColorFromRGB(0, 153, 204),
        touchgfx::Color::getColorFromRGB(0, 51, 102),
        touchgfx::Color::getColorFromRGB(51, 102, 153));
    flexButton2.invalidate();

    flexButton3.setBoxWithBorderColors(
        (currentIndex == 2) ? touchgfx::Color::getColorFromRGB(255, 0, 0)
                            : touchgfx::Color::getColorFromRGB(0, 102, 153),
        touchgfx::Color::getColorFromRGB(0, 153, 204),
        touchgfx::Color::getColorFromRGB(0, 51, 102),
        touchgfx::Color::getColorFromRGB(51, 102, 153));
    flexButton3.invalidate();
}

void Chosing_modeView::handleTickEvent()
{
    static bool firstTick = true;

    static uint8_t lastPB10;
    static uint8_t lastPB12;
    static uint8_t lastPA0;

    uint8_t PB10 = HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_10);
    uint8_t PB12 = HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_12);
    uint8_t PA0 = HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_0);

    if (firstTick)
    {
        lastPB10 = PB10;
        lastPB12 = PB12;
        lastPA0 = PA0;
        firstTick = false;
        return;
    }

    if (PB10 == GPIO_PIN_RESET && lastPB10 == GPIO_PIN_SET)
    {
        moveUp();
    }
    lastPB10 = PB10;

    if (PB12 == GPIO_PIN_RESET && lastPB12 == GPIO_PIN_SET)
    {
        moveDown();
    }
    lastPB12 = PB12;

    if (PA0 == GPIO_PIN_RESET && lastPA0 == GPIO_PIN_SET)
    {
        selectMode();
    }
    lastPA0 = PA0;
}
