#ifndef TILE_LETTER_HPP
#define TILE_LETTER_HPP

#include <gui_generated/containers/Tile_letterBase.hpp>
#include <touchgfx/Application.hpp>
class Tile_letter : public Tile_letterBase
{
public:
    Tile_letter();
    virtual ~Tile_letter() {}
    void setValue(uint16_t value);
    virtual void initialize();
    uint16_t getValue() const;
    void animateSpawn();
    virtual void handleTickEvent();
    int centerX, centerY;
protected:
    int currentStep = 0;
    static const int totalSteps = 6;
    uint16_t storedValue = 0xFFFF;  //luu trang thai chu cai
};

#endif // TILE_LETTER_HPP
