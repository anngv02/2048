#ifndef TILE_HPP
#define TILE_HPP

#include <gui_generated/containers/TileBase.hpp>
#include <touchgfx/Application.hpp>
class Tile : public TileBase
{
public:
    Tile();
    virtual ~Tile() {}
    void setValue(uint16_t value);
    virtual void initialize();
    uint16_t getValue() const;
    void animateSpawn();
    virtual void handleTickEvent();
    int centerX, centerY;  
protected:
    int currentStep = 0;
    static const int totalSteps = 6;
};

#endif // TILE_HPP
