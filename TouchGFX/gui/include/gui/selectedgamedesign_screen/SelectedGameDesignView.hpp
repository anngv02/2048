#ifndef SELECTEDGAMEDESIGNVIEW_HPP
#define SELECTEDGAMEDESIGNVIEW_HPP

#include <gui_generated/selectedgamedesign_screen/SelectedGameDesignViewBase.hpp>
#include <gui/selectedgamedesign_screen/SelectedGameDesignPresenter.hpp>

/**
 * @class SelectedGameDesignView
 * @brief View cho màn hình SelectedGameDesign (Menu chọn kích thước game)
 *
 * Các lựa chọn:
 * - 0: Game 3x3
 * - 1: Game 4x4 (MainScreen)
 * - 2: Game 5x5
 * - 3: Quay về Chosing_mode
 *
 * Điều khiển:
 * - UP (PE2): Di chuyển lên
 * - DOWN (PE3): Di chuyển xuống
 * - SELECT (PA0): Chọn mode
 * - Touch: Nhấn trực tiếp vào button
 */
class SelectedGameDesignView : public SelectedGameDesignViewBase
{
public:
    SelectedGameDesignView();
    virtual ~SelectedGameDesignView() {}
    virtual void setupScreen();
    virtual void tearDownScreen();
    virtual void handleTickEvent();

    // ==============================================================================
    // Public methods để Presenter gọi (MVP pattern)
    // ==============================================================================
    void onMoveUp();      // Di chuyển lên trong menu
    void onMoveDown();    // Di chuyển xuống trong menu
    void onSelectMode();  // Chọn mode hiện tại (PA0)

protected:
    void moveUp();
    void moveDown();
    void selectMode();
    void updateHighlight();
    uint8_t currentIndex;
    static const uint8_t MODE_COUNT = 4; // Có 4 nút: btn3x3, btn4x4, btn5x5, flexButton1
};

#endif // SELECTEDGAMEDESIGNVIEW_HPP
