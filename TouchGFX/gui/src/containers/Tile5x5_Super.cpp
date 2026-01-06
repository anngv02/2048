#include <gui/containers/Tile5x5_Super.hpp>
#include <touchgfx/Unicode.hpp> 
#include <touchgfx/Color.hpp>
Tile5x5_Super::Tile5x5_Super()
{

}

void Tile5x5_Super::initialize()
{
    Tile5x5_SuperBase::initialize();
}
void Tile5x5_Super::setValue(uint16_t value)
{
    if (value == 0)
    {   
        colortype color1 = Color::getColorFromRGB(238, 228, 218); // default for 2
        textArea1.setVisible(false);
        boxWithBorder1.setColor(color1);
        boxWithBorder1.setAlpha(35);
    }
    else
    {
        Unicode::snprintf(textArea1Buffer, TEXTAREA1_SIZE, "%u", value);
        textArea1.setVisible(true);
        boxWithBorder1.setVisible(true);
        colortype color = Color::getColorFromRGB(238, 228, 218); // default for 2
        switch (value)
        {
        case 2:   color = Color::getColorFromRGB(238, 228, 218); break;
        case 4:   color = Color::getColorFromRGB(237, 224, 200); break;
        case 8:   color = Color::getColorFromRGB(242, 177, 121); break;
        case 16:  color = Color::getColorFromRGB(245, 149, 99); break;
        case 32:  color = Color::getColorFromRGB(246, 124, 95); break;
        case 64:  color = Color::getColorFromRGB(246, 94, 59); break;
        case 128: color = Color::getColorFromRGB(237, 207, 114); break;
        case 256: color = Color::getColorFromRGB(237,204, 97); break;
        case 512: color =  Color::getColorFromRGB(237,200, 80); break;
        default:  color = Color::getColorFromRGB(60, 58, 50); break; // màu cho >512
        }
        boxWithBorder1.setColor(color);
        boxWithBorder1.setAlpha(200);
    }
    boxWithBorder1.invalidate();
    textArea1.invalidate();
}
uint16_t Tile5x5_Super::getValue() const
{
    if (!textArea1.isVisible())
        return 0;

    return touchgfx::Unicode::atoi(textArea1Buffer);
}
void Tile5x5_Super::animateSpawn()
{
    currentStep = 0;

    // bắt đầu nhỏ
    setWidthHeight(10, 10);
    moveTo(centerX - 5, centerY - 5);
    setVisible(true);
    boxWithBorder1.setVisible(true);
    textArea1.setVisible(true);

    invalidate();
    Application::getInstance()->registerTimerWidget(this); // gọi tickEvent
}

void Tile5x5_Super::handleTickEvent()
{
    if (currentStep >= totalSteps)
    {
        // Hoàn tất animation
        setWidthHeight(48, 48);
        moveTo(centerX - 24, centerY - 24);
        invalidate();
        Application::getInstance()->unregisterTimerWidget(this);
        return;
    }

    int size = 10 + ((48 - 10) * currentStep) / totalSteps;
    int offset = size / 2;
    setWidthHeight(size, size);
    moveTo(centerX - offset, centerY - offset);
    invalidate();

    currentStep++;
}