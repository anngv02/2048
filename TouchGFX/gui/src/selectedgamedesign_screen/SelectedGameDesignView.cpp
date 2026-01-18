#include <gui/selectedgamedesign_screen/SelectedGameDesignView.hpp>
#include <touchgfx/Color.hpp>

// ==============================================================================
// LƯU Ý: View KHÔNG truy cập GPIO trực tiếp - tuân thủ MVP pattern
// GPIO polling được xử lý trong Model::tick()
// Các chân GPIO mới:
//   - PE2 (UP): Di chuyển lên
//   - PE3 (DOWN): Di chuyển xuống
//   - PA0 (BACK/SELECT): Chọn mode
// ==============================================================================

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

// ==============================================================================
// INTERNAL METHODS - Logic di chuyển menu
// ==============================================================================

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
    // Highlight nút đang được chọn (màu đỏ), các nút khác màu xanh
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

// ==============================================================================
// PUBLIC METHODS - ĐƯỢC GỌI TỪ PRESENTER (MVP PATTERN)
// ==============================================================================

/**
 * @brief Xử lý khi Presenter báo nhấn nút UP (PE2)
 */
void SelectedGameDesignView::onMoveUp()
{
    moveUp();
}

/**
 * @brief Xử lý khi Presenter báo nhấn nút DOWN (PE3)
 */
void SelectedGameDesignView::onMoveDown()
{
    moveDown();
}

/**
 * @brief Xử lý khi Presenter báo nhấn nút SELECT (PA0)
 */
void SelectedGameDesignView::onSelectMode()
{
    selectMode();
}

/**
 * @brief handleTickEvent - GPIO polling đã được chuyển sang Model
 */
void SelectedGameDesignView::handleTickEvent()
{
    // GPIO polling được xử lý trong Model::tick()
    // View nhận events thông qua Presenter (MVP pattern)
}
