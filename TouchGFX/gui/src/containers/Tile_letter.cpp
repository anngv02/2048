#include <gui/containers/Tile_letter.hpp>
#include <touchgfx/Unicode.hpp>
#include <touchgfx/Color.hpp>
Tile_letter::Tile_letter()
{

}

void Tile_letter::initialize()
{
    Tile_letterBase::initialize();
    textArea1.setWildcard(textArea1Buffer);
}
void Tile_letter::setValue(uint16_t value)
{   
    storedValue = value; // Lưu lại giá trị để sử dụng sau
    if (value == 0XFFFF) // dùng 0xFFFF làm mã cho tile trống
    {
        textArea1.setVisible(false);
        boxWithBorder1.setAlpha(35);
        boxWithBorder1.setColor(Color::getColorFromRGB(200, 200, 200)); // Màu nhạt
    }
    else
    {
        char letter = 'A' + value; // Chuyển giá trị 0 -> 'A', 1 -> 'B', ...
        Unicode::snprintf(textArea1Buffer, TEXTAREA1_SIZE, "%c", letter);
        textArea1.setVisible(true);

        // Đổi màu theo giá trị chữ
        colortype color = Color::getColorFromRGB(238, 228, 218);
        switch (value)
        {
            case 0: color = Color::getColorFromRGB(238, 228, 218); break; // A
            case 1: color = Color::getColorFromRGB(237, 224, 200); break; // B
            case 2: color = Color::getColorFromRGB(242, 177, 121); break; // C
            case 3: color = Color::getColorFromRGB(245, 149, 99); break;  // D
            case 4: color = Color::getColorFromRGB(246, 124, 95); break;  // E
            case 5: color = Color::getColorFromRGB(246, 94, 59); break;   // F
            case 6: color = Color::getColorFromRGB(237, 207, 114); break; // G
            case 7: color = Color::getColorFromRGB(237,204, 97); break;   // H
            case 8: color = Color::getColorFromRGB(237,200, 80); break;   // I
            case 9: color = Color::getColorFromRGB(60, 58, 50); break;    // J
            default: color = Color::getColorFromRGB(100, 100, 100); break; // K+
        }

        boxWithBorder1.setColor(color);
        boxWithBorder1.setAlpha(200);
    }

    boxWithBorder1.invalidate();
    textArea1.invalidate();
}
uint16_t Tile_letter::getValue() const
{
    return storedValue;
}
void Tile_letter::animateSpawn()
{
    currentStep = 0;
    setWidthHeight(10, 10);
    moveTo(centerX - 5, centerY - 5);
    setVisible(true);
    boxWithBorder1.setVisible(true);
    textArea1.setVisible(true);
    invalidate();
    Application::getInstance()->registerTimerWidget(this);
}
void Tile_letter::handleTickEvent()
{
    if (currentStep >= totalSteps)
    {
        setWidthHeight(60, 60);
        moveTo(centerX - 30, centerY - 30);
        invalidate();
        Application::getInstance()->unregisterTimerWidget(this);
        return;
    }

    int size = 10 + ((60 - 10) * currentStep) / totalSteps;
    int offset = size / 2;
    setWidthHeight(size, size);
    moveTo(centerX - offset, centerY - offset);
    invalidate();
    currentStep++;
}