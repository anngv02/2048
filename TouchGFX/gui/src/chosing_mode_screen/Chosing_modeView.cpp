#include <gui/chosing_mode_screen/Chosing_modeView.hpp>
#include <touchgfx/Color.hpp>

// ==============================================================================
// LƯU Ý: View KHÔNG truy cập GPIO trực tiếp - tuân thủ MVP pattern
// GPIO polling được xử lý trong Model::tick()
// Các chân GPIO mới:
//   - PE2 (UP): Di chuyển lên
//   - PE3 (DOWN): Di chuyển xuống
//   - PA0 (BACK/SELECT): Chọn mode
// ==============================================================================

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

// ==============================================================================
// INTERNAL METHODS - Logic di chuyển menu
// ==============================================================================

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
    // Highlight nút đang được chọn (màu đỏ), các nút khác màu xanh
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

// ==============================================================================
// PUBLIC METHODS - ĐƯỢC GỌI TỪ PRESENTER (MVP PATTERN)
// ==============================================================================

/**
 * @brief Xử lý khi Presenter báo nhấn nút UP (PE2)
 */
void Chosing_modeView::onMoveUp()
{
    moveUp();
}

/**
 * @brief Xử lý khi Presenter báo nhấn nút DOWN (PE3)
 */
void Chosing_modeView::onMoveDown()
{
    moveDown();
}

/**
 * @brief Xử lý khi Presenter báo nhấn nút SELECT (PA0)
 */
void Chosing_modeView::onSelectMode()
{
    selectMode();
}

/**
 * @brief handleTickEvent - GPIO polling đã được chuyển sang Model
 */
void Chosing_modeView::handleTickEvent()
{
    // GPIO polling được xử lý trong Model::tick()
    // View nhận events thông qua Presenter (MVP pattern)
}
