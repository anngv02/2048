#ifndef MODEL_HPP
#define MODEL_HPP

#include <stdint.h>

class ModelListener;

/**
 * @class Model
 * @brief Model trong kiến trúc MVP - xử lý hardware và business logic
 * 
 * Model chịu trách nhiệm:
 * - Polling GPIO buttons (PE2, PE3, PE4, PE5, PE6, PA0)
 * - Polling ADC joystick với HYSTERESIS (ADC1 - X, ADC3 - Y)
 * - Software debounce để chống rung phím
 * - Điều khiển Buzzer (PC4) non-blocking
 * - Notify Presenter khi có sự kiện
 * 
 * ==============================================================================
 * GPIO PIN MAPPING:
 * ==============================================================================
 * Button UP:     PE2 (Input, Pull-up, Active-LOW)
 * Button DOWN:   PE3 (Input, Pull-up, Active-LOW)
 * Button LEFT:   PE4 (Input, Pull-up, Active-LOW)
 * Button RIGHT:  PE5 (Input, Pull-up, Active-LOW)
 * Button JOY_BTN:PE6 (Input, Pull-up, Active-LOW) -> BACK (same as PA0)
 * Button BACK:   PA0 (Input, No-Pull, Active-HIGH, Rising Edge)
 * Buzzer:        PC4 (Output, Active-HIGH)
 * 
 * ==============================================================================
 * ADC PIN MAPPING (Joystick):
 * ==============================================================================
 * Joystick X:    PA5 (ADC1, Channel 5)
 * Joystick Y:    PF6 (ADC3, Channel 4)
 * 
 * ==============================================================================
 * JOYSTICK HYSTERESIS:
 * ==============================================================================
 * Để tránh buzzer kêu nhiều lần khi giá trị dao động quanh ngưỡng:
 * 
 * Trigger LOW:  ADC < MIN_THRESHOLD (1000)
 * Reset LOW:    ADC > MIN_THRESHOLD + RESET_OFFSET (1500)
 * 
 * Trigger HIGH: ADC > MAX_THRESHOLD (3000)
 * Reset HIGH:   ADC < MAX_THRESHOLD - RESET_OFFSET (2500)
 */
class Model
{
public:
    Model();

    /**
     * @brief Bind ModelListener (Presenter) để nhận notifications
     * @param listener Con trỏ đến ModelListener
     */
    void bind(ModelListener* listener)
    {
        modelListener = listener;
    }

    /**
     * @brief Hàm tick được gọi mỗi frame (~16ms)
     * Thực hiện polling GPIO, ADC và xử lý buzzer
     */
    void tick();

    /**
     * @brief Bật buzzer beep ngắn (non-blocking)
     * @param durationMs Thời gian beep (ms), mặc định 100ms
     */
    void buzzerBeep(uint32_t durationMs = 100);

protected:
    ModelListener* modelListener;

private:
    // ==================== GPIO Button State ====================
    uint8_t buttonState;        // Trạng thái hiện tại
    uint8_t lastButtonState;    // Trạng thái trước đó (để detect edge)
    
    // ==================== Software Debounce ====================
    uint32_t lastPressTime;
    static const uint32_t DEBOUNCE_DELAY_MS = 150;
    
    // ==================== Buzzer Control ====================
    bool buzzerActive;
    uint32_t buzzerStartTime;
    uint32_t buzzerDuration;
    
    // ==================== Button Bit Masks ====================
    static const uint8_t BTN_UP_MASK      = 0x01;  // Bit 0: PE2 - UP
    static const uint8_t BTN_DOWN_MASK    = 0x02;  // Bit 1: PE3 - DOWN
    static const uint8_t BTN_LEFT_MASK    = 0x04;  // Bit 2: PE4 - LEFT
    static const uint8_t BTN_RIGHT_MASK   = 0x08;  // Bit 3: PE5 - RIGHT
    static const uint8_t BTN_JOY_BTN_MASK = 0x10;  // Bit 4: PE6 - JOY_BTN (SELECT)
    static const uint8_t BTN_BACK_MASK    = 0x20;  // Bit 5: PA0 - BACK

    // ==================== Joystick ADC Values ====================
    uint16_t joystickX;  // ADC1 - PA5
    uint16_t joystickY;  // ADC3 - PF6
    
    // ==================== Joystick Hysteresis Thresholds ====================
    // Ngưỡng kích hoạt
    static const uint16_t MIN_THRESHOLD = 1000;   // Trigger khi ADC < 1000
    static const uint16_t MAX_THRESHOLD = 3000;   // Trigger khi ADC > 3000
    
    // Offset để reset (tạo vùng hysteresis)
    static const uint16_t RESET_OFFSET = 500;
    // Reset LOW:  ADC > MIN_THRESHOLD + RESET_OFFSET = 1500
    // Reset HIGH: ADC < MAX_THRESHOLD - RESET_OFFSET = 2500
    
    // ==================== Joystick Active Flags (Hysteresis) ====================
    // Cờ khóa trạng thái - true = đang active, phải quay về vùng an toàn mới reset
    bool isJoyUpActive;      // Joystick Y > MAX (gạt lên)
    bool isJoyDownActive;    // Joystick Y < MIN (gạt xuống)
    bool isJoyLeftActive;    // Joystick X > MAX (gạt trái)
    bool isJoyRightActive;   // Joystick X < MIN (gạt phải)

    // ==================== Private Methods ====================
    /**
     * @brief Đọc trạng thái tất cả GPIO buttons
     * @return Bitmask trạng thái các nút
     */
    uint8_t readButtonsGPIO();
    
    /**
     * @brief Đọc giá trị ADC Joystick X (PA5 - ADC1)
     * @return Giá trị ADC 12-bit (0-4095)
     */
    uint16_t readJoystickX();
    
    /**
     * @brief Đọc giá trị ADC Joystick Y (PF6 - ADC3)
     * @return Giá trị ADC 12-bit (0-4095)
     */
    uint16_t readJoystickY();
    
    /**
     * @brief Xử lý logic Joystick với Hysteresis
     * @param currentTime Thời gian hiện tại (HAL_GetTick)
     */
    void processJoystickWithHysteresis(uint32_t currentTime);
    
    /**
     * @brief Xử lý logic Digital Buttons với Debounce
     * @param currentState Trạng thái GPIO hiện tại
     * @param currentTime Thời gian hiện tại
     */
    void processDigitalButtons(uint8_t currentState, uint32_t currentTime);
    
    /**
     * @brief Xử lý buzzer trong tick (non-blocking)
     */
    void processBuzzer();
    
    void buzzerOn();
    void buzzerOff();
};

#endif // MODEL_HPP
