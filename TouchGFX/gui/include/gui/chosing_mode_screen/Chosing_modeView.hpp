#ifndef CHOSING_MODEVIEW_HPP
#define CHOSING_MODEVIEW_HPP

#include <gui_generated/chosing_mode_screen/Chosing_modeViewBase.hpp>
#include <gui/chosing_mode_screen/Chosing_modePresenter.hpp>

class Chosing_modeView : public Chosing_modeViewBase
{
public:
    Chosing_modeView();
    virtual ~Chosing_modeView() {}
    virtual void setupScreen();
    virtual void tearDownScreen();
    virtual void handleTickEvent();
protected:
    void moveUp();
    void moveDown();
    void selectMode();
    void updateHighlight();
    uint8_t currentIndex;
    static const uint8_t MODE_COUNT = 4; // Có 4 nút: btn3x3, btn4x4, btn5x5, flexButton1
};

#endif // CHOSING_MODEVIEW_HPP
