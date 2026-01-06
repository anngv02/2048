#include <gui/selectedgamedesign_screen/SelectedGameDesignView.hpp>
#include <touchgfx/Color.hpp>
#include <stm32f4xx_hal.h>

SelectedGameDesignView::SelectedGameDesignView()
    : SelectedGameDesignViewBase(),
      currentIndex(0)
{
}

void SelectedGameDesignView::setupScreen()
{
    SelectedGameDesignViewBase::setupScreen();
    currentIndex = 0;
    updateHighlight();
}

void SelectedGameDesignView::tearDownScreen()
{
    SelectedGameDesignViewBase::tearDownScreen();
}

void SelectedGameDesignView::moveUp()
{
    if (currentIndex > 0)
    {
        currentIndex--;
    }
    else
    {
        currentIndex = MODE_COUNT - 1;
    }
    updateHighlight();
}

void SelectedGameDesignView::moveDown()
{
    if (currentIndex < MODE_COUNT - 1)
    {
        currentIndex++;
    }
    else
    {
        currentIndex = 0;
    }
    updateHighlight();
}

void SelectedGameDesignView::selectMode()
{
    if (currentIndex == 0)
    {
        application().gotoScreen3x3ScreenCoverTransitionEast();
    }
    else if (currentIndex == 1)
    {
        application().gotoMainScreenScreenCoverTransitionEast();
    }
    else if (currentIndex == 2)
    {
        application().gotoScreen5x5ScreenCoverTransitionEast();
    }
    else if (currentIndex == 3)
    {
        application().gotoChosing_modeScreenWipeTransitionEast();
    }
}

void SelectedGameDesignView::updateHighlight()
{
    btn3x3.setBoxWithBorderColors(
        (currentIndex == 0) ? touchgfx::Color::getColorFromRGB(255, 0, 0)
                            : touchgfx::Color::getColorFromRGB(0, 102, 153),
        touchgfx::Color::getColorFromRGB(0, 153, 204),
        touchgfx::Color::getColorFromRGB(0, 51, 102),
        touchgfx::Color::getColorFromRGB(51, 102, 153));
    btn3x3.invalidate();

    btn4x4.setBoxWithBorderColors(
        (currentIndex == 1) ? touchgfx::Color::getColorFromRGB(255, 0, 0)
                            : touchgfx::Color::getColorFromRGB(0, 102, 153),
        touchgfx::Color::getColorFromRGB(0, 153, 204),
        touchgfx::Color::getColorFromRGB(0, 51, 102),
        touchgfx::Color::getColorFromRGB(51, 102, 153));
    btn4x4.invalidate();

    btn5x5.setBoxWithBorderColors(
        (currentIndex == 2) ? touchgfx::Color::getColorFromRGB(255, 0, 0)
                            : touchgfx::Color::getColorFromRGB(0, 102, 153),
        touchgfx::Color::getColorFromRGB(0, 153, 204),
        touchgfx::Color::getColorFromRGB(0, 51, 102),
        touchgfx::Color::getColorFromRGB(51, 102, 153));
    btn5x5.invalidate();

    flexButton1.setBoxWithBorderColors(
        (currentIndex == 3) ? touchgfx::Color::getColorFromRGB(255, 0, 0)
                            : touchgfx::Color::getColorFromRGB(0, 102, 153),
        touchgfx::Color::getColorFromRGB(0, 153, 204),
        touchgfx::Color::getColorFromRGB(0, 51, 102),
        touchgfx::Color::getColorFromRGB(51, 102, 153));
    flexButton1.invalidate();
}

void SelectedGameDesignView::handleTickEvent()
{
    static bool firstTick = true;
    static uint8_t lastUpBtn;
    static uint8_t lastDownBtn;
    static uint8_t lastSelectBtn;

    uint8_t upBtn = HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_10);
    uint8_t downBtn = HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_12);
    uint8_t selectBtn = HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_0);

    if (firstTick)
    {
        lastUpBtn = upBtn;
        lastDownBtn = downBtn;
        lastSelectBtn = selectBtn;
        firstTick = false;
        return;
    }

    if (upBtn == GPIO_PIN_RESET && lastUpBtn == GPIO_PIN_SET)
    {
        moveUp();
    }
    lastUpBtn = upBtn;

    if (downBtn == GPIO_PIN_RESET && lastDownBtn == GPIO_PIN_SET)
    {
        moveDown();
    }
    lastDownBtn = downBtn;

    if (selectBtn == GPIO_PIN_RESET && lastSelectBtn == GPIO_PIN_SET)
    {
        selectMode();
    }
    lastSelectBtn = selectBtn;
}
