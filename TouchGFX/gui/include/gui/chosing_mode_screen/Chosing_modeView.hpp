#ifndef CHOSING_MODEVIEW_HPP
#define CHOSING_MODEVIEW_HPP

#include <gui_generated/chosing_mode_screen/Chosing_modeViewBase.hpp>
#include <gui/chosing_mode_screen/Chosing_modePresenter.hpp>

/**
 * @class Chosing_modeView
 * @brief View cho màn hình Chosing_mode (Menu chọn chế độ game)
 *
 * Điều khiển:
 * - UP (PE2): Di chuyển lên trong menu
 * - DOWN (PE3): Di chuyển xuống trong menu
 * - BACK/SELECT (PA0): Chọn mode hiện tại
 * - Touch: Nhấn trực tiếp vào nút trên màn hình
 */
class Chosing_modeView : public Chosing_modeViewBase
{
public:
    Chosing_modeView();
    virtual ~Chosing_modeView() {}
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
    static const uint8_t MODE_COUNT = 3;
};

#endif // CHOSING_MODEVIEW_HPP
