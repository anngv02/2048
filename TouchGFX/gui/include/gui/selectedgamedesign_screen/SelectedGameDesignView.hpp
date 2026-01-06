#ifndef SELECTEDGAMEDESIGNVIEW_HPP
#define SELECTEDGAMEDESIGNVIEW_HPP

#include <gui_generated/selectedgamedesign_screen/SelectedGameDesignViewBase.hpp>
#include <gui/selectedgamedesign_screen/SelectedGameDesignPresenter.hpp>

class SelectedGameDesignView : public SelectedGameDesignViewBase
{
public:
    SelectedGameDesignView();
    virtual ~SelectedGameDesignView() {}
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

#endif // SELECTEDGAMEDESIGNVIEW_HPP
