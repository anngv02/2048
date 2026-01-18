#ifndef MODELLISTENER_HPP
#define MODELLISTENER_HPP

#include <gui/model/Model.hpp>

/**
 * @class ModelListener
 * @brief Interface để Presenter nhận notifications từ Model
 *
 * Trong kiến trúc MVP:
 * - Model phát hiện hardware events (button press, joystick move)
 * - Model gọi các virtual functions của ModelListener
 * - Presenter (kế thừa ModelListener) override các hàm này
 * - Presenter sau đó gọi View để cập nhật UI
 *
 * Luồng: Hardware -> Model -> ModelListener(Presenter) -> View
 * 
 * ==============================================================================
 * BUTTON EVENTS:
 * ==============================================================================
 * onButtonUp():     PE2 pressed OR Joystick Y > 3000
 * onButtonDown():   PE3 pressed OR Joystick Y < 1000
 * onButtonLeft():   PE4 pressed OR Joystick X > 3000
 * onButtonRight():  PE5 pressed OR Joystick X < 1000
 * onButtonBack():   PA0 pressed OR PE6 (Joystick button) pressed
 */
class ModelListener
{
public:
    ModelListener() : model(0) {}
    
    virtual ~ModelListener() {}

    /**
     * @brief Bind Model để có thể gọi ngược lại (nếu cần)
     * @param m Con trỏ đến Model
     */
    void bind(Model* m)
    {
        model = m;
    }

    // ==============================================================================
    // Virtual functions cho Button/Joystick Events
    // ==============================================================================

    /**
     * @brief Được gọi khi nhấn nút UP (PE2) HOẶC gạt joystick lên (Y > 3000)
     */
    virtual void onButtonUp() {}

    /**
     * @brief Được gọi khi nhấn nút DOWN (PE3) HOẶC gạt joystick xuống (Y < 1000)
     */
    virtual void onButtonDown() {}

    /**
     * @brief Được gọi khi nhấn nút LEFT (PE4) HOẶC gạt joystick trái (X > 3000)
     */
    virtual void onButtonLeft() {}

    /**
     * @brief Được gọi khi nhấn nút RIGHT (PE5) HOẶC gạt joystick phải (X < 1000)
     */
    virtual void onButtonRight() {}

    /**
     * @brief Được gọi khi nhấn nút BACK (PA0) HOẶC nút Joystick (PE6)
     */
    virtual void onButtonBack() {}

    // ==============================================================================
    // Utility methods cho Presenter/View sử dụng
    // ==============================================================================

    /**
     * @brief Kích hoạt buzzer beep khi Game Over (1 giây)
     * View gọi method này qua presenter trước khi chuyển màn hình
     */
    void notifyGameOver()
    {
        if (model)
        {
            model->buzzerBeep(1000);  // Beep 1 giây
        }
    }

protected:
    Model* model;
};

#endif // MODELLISTENER_HPP
