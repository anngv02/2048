#ifndef TILE5X5_SUPER_HPP
#define TILE5X5_SUPER_HPP

#include <gui_generated/containers/Tile5x5_SuperBase.hpp>
#include <touchgfx/mixins/MoveAnimator.hpp>
#include <touchgfx/Application.hpp>
class Tile5x5_Super : public Tile5x5_SuperBase
{
public:
    Tile5x5_Super();
    virtual ~Tile5x5_Super() {}
    void setValue(uint16_t value);
    virtual void initialize();
    uint16_t getValue() const;

    // Thêm hàm khởi động animation
    void animateTo(int x, int y);

     //hieu ung spawn
    void animateSpawn();
    virtual void handleTickEvent();
    int centerX, centerY;  
protected:
    int currentStep = 0;
    static const int totalSteps = 6;
};

#endif // TILE5X5_SUPER_HPP
