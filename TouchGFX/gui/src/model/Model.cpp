#include <gui/model/Model.hpp>
#include <gui/model/ModelListener.hpp>

// Include HAL và main.h để sử dụng GPIO/ADC defines từ CubeMX
extern "C" {
#include "stm32f4xx_hal.h"
#include "main.h"  // Chứa: UP_Pin, DOWN_Pin, LEFT_Pin, RIGHT_Pin, JOY_SW_Pin, BUZZER_Pin
}

/**
 * ==============================================================================
 * EXTERN ADC HANDLES (Được khai báo trong main.c bởi CubeMX)
 * ==============================================================================
 */
extern ADC_HandleTypeDef hadc1;  // ADC1 - PA5 - Joystick X
extern ADC_HandleTypeDef hadc3;  // ADC3 - PF6 - Joystick Y

/**
 * ==============================================================================
 * GPIO PIN ALIASES
 * ==============================================================================
 */
#define BTN_BACK_PORT   GPIOA
#define BTN_BACK_PIN    GPIO_PIN_0

/**
 * ==============================================================================
 * CONSTRUCTOR
 * ==============================================================================
 */
Model::Model() :
    modelListener(0),
    buttonState(0),
    lastButtonState(0x3F),  // Tất cả HIGH (không nhấn) để tránh false trigger khi khởi động
    lastPressTime(0),
    buzzerActive(false),
    buzzerStartTime(0),
    buzzerDuration(0),
    joystickX(2048),        // Giá trị center mặc định
    joystickY(2048),
    isJoyUpActive(false),
    isJoyDownActive(false),
    isJoyLeftActive(false),
    isJoyRightActive(false)
{
    buzzerOff();
}

/**
 * ==============================================================================
 * TICK - Main processing loop (~16ms per frame)
 * ==============================================================================
 * Luồng xử lý:
 * 1. Đọc tất cả sensors (GPIO + ADC)
 * 2. Xử lý Digital Buttons với Debounce
 * 3. Xử lý Joystick với Hysteresis
 * 4. Notify Presenter qua ModelListener
 * 5. Xử lý Buzzer non-blocking
 */
void Model::tick()
{
    // ========== 1. ĐỌC TẤT CẢ SENSORS ==========
    uint8_t currentState = readButtonsGPIO();
    joystickX = readJoystickX();
    joystickY = readJoystickY();
    
    uint32_t currentTime = HAL_GetTick();

    // ========== 2. XỬ LÝ DIGITAL BUTTONS (với Debounce) ==========
    processDigitalButtons(currentState, currentTime);

    // ========== 3. XỬ LÝ JOYSTICK (với Hysteresis) ==========
    processJoystickWithHysteresis(currentTime);

    // ========== 4. LƯU TRẠNG THÁI CHO TICK SAU ==========
    lastButtonState = currentState;

    // ========== 5. XỬ LÝ BUZZER ==========
    processBuzzer();
}

/**
 * ==============================================================================
 * PROCESS DIGITAL BUTTONS - Xử lý nút bấm với Debounce
 * ==============================================================================
 * - PE2-PE5: Active-LOW (Pull-up) -> Falling edge = nhấn
 * - PE6 (JOY_BTN): Active-LOW -> Falling edge = nhấn -> SELECT/ENTER
 * - PA0: Active-HIGH -> Rising edge = nhấn -> BACK
 */
void Model::processDigitalButtons(uint8_t currentState, uint32_t currentTime)
{
    // Chỉ xử lý nếu đã qua khoảng debounce
    if (currentTime - lastPressTime < DEBOUNCE_DELAY_MS)
    {
        return;
    }

    // -------------------- UP (PE2) --------------------
    // Falling edge: lastState = HIGH (1), currentState = LOW (0)
    if (!(currentState & BTN_UP_MASK) && (lastButtonState & BTN_UP_MASK))
    {
        buzzerBeep(100);
        if (modelListener) modelListener->onButtonUp();
        lastPressTime = currentTime;
        return;
    }

    // -------------------- DOWN (PE3) --------------------
    if (!(currentState & BTN_DOWN_MASK) && (lastButtonState & BTN_DOWN_MASK))
    {
        buzzerBeep(100);
        if (modelListener) modelListener->onButtonDown();
        lastPressTime = currentTime;
        return;
    }

    // -------------------- LEFT (PE4) --------------------
    if (!(currentState & BTN_LEFT_MASK) && (lastButtonState & BTN_LEFT_MASK))
    {
        buzzerBeep(100);
        if (modelListener) modelListener->onButtonLeft();
        lastPressTime = currentTime;
        return;
    }

    // -------------------- RIGHT (PE5) --------------------
    if (!(currentState & BTN_RIGHT_MASK) && (lastButtonState & BTN_RIGHT_MASK))
    {
        buzzerBeep(100);
        if (modelListener) modelListener->onButtonRight();
        lastPressTime = currentTime;
        return;
    }

    // -------------------- BACK: JOY_BTN (PE6) OR PA0 --------------------
    // JOY_BTN (PE6): Falling edge (Pull-up: HIGH->LOW = nhấn)
    // PA0: Rising edge (Active-HIGH: LOW->HIGH = nhấn)
    bool joyBtnPressed = !(currentState & BTN_JOY_BTN_MASK) && (lastButtonState & BTN_JOY_BTN_MASK);
    bool backBtnPressed = (currentState & BTN_BACK_MASK) && !(lastButtonState & BTN_BACK_MASK);
    
    if (joyBtnPressed || backBtnPressed)
    {
        buzzerBeep(100);
        if (modelListener) modelListener->onButtonBack();
        lastPressTime = currentTime;
        return;
    }
}

/**
 * ==============================================================================
 * PROCESS JOYSTICK WITH HYSTERESIS - Chống nhiễu/Chống lặp Buzzer
 * ==============================================================================
 * 
 * HYSTERESIS MECHANISM:
 * ----------------------
 * Thay vì dùng 1 ngưỡng (ví dụ < 1000), ta dùng 2 ngưỡng:
 * - Trigger threshold: < MIN_THRESHOLD (1000)
 * - Reset threshold:   > MIN_THRESHOLD + RESET_OFFSET (1500)
 * 
 * Điều này tạo vùng "chết" giữa 1000-1500, nơi không có action nào xảy ra,
 * ngăn việc buzzer kêu liên tục khi giá trị dao động.
 * 
 * FLOW:
 * 1. Joystick ở center (2048) -> không làm gì
 * 2. Gạt xuống, ADC < 1000 -> isJoyDownActive = true, trigger DOWN, buzzer beep
 * 3. ADC dao động 900-1100 -> không trigger thêm vì isJoyDownActive = true
 * 4. Thả về, ADC > 1500 -> isJoyDownActive = false (reset)
 * 5. Có thể trigger lại nếu gạt xuống một lần nữa
 * 
 * Mapping (theo yêu cầu đã điều chỉnh):
 * - Trục Y > MAX (3000): UP
 * - Trục Y < MIN (1000): DOWN
 * - Trục X > MAX (3000): LEFT
 * - Trục X < MIN (1000): RIGHT
 */
void Model::processJoystickWithHysteresis(uint32_t currentTime)
{
    // Chỉ xử lý nếu đã qua khoảng debounce (tránh spam quá nhanh)
    if (currentTime - lastPressTime < DEBOUNCE_DELAY_MS)
    {
        return;
    }

    // ==================== JOYSTICK Y AXIS (UP/DOWN) ====================
    
    // -------------------- UP: Y > MAX_THRESHOLD --------------------
    if (!isJoyUpActive)
    {
        // Chưa active -> kiểm tra trigger
        if (joystickY > MAX_THRESHOLD)
        {
            isJoyUpActive = true;  // Khóa trạng thái
            buzzerBeep(100);
            if (modelListener) modelListener->onButtonUp();
            lastPressTime = currentTime;
        }
    }
    else
    {
        // Đang active -> kiểm tra reset
        // Reset khi Y < MAX_THRESHOLD - RESET_OFFSET (2500)
        if (joystickY < (MAX_THRESHOLD - RESET_OFFSET))
        {
            isJoyUpActive = false;  // Mở khóa, cho phép trigger lại
        }
    }

    // -------------------- DOWN: Y < MIN_THRESHOLD --------------------
    if (!isJoyDownActive)
    {
        if (joystickY < MIN_THRESHOLD)
        {
            isJoyDownActive = true;
            buzzerBeep(100);
            if (modelListener) modelListener->onButtonDown();
            lastPressTime = currentTime;
        }
    }
    else
    {
        // Reset khi Y > MIN_THRESHOLD + RESET_OFFSET (1500)
        if (joystickY > (MIN_THRESHOLD + RESET_OFFSET))
        {
            isJoyDownActive = false;
        }
    }

    // ==================== JOYSTICK X AXIS (LEFT/RIGHT) ====================
    
    // -------------------- LEFT: X > MAX_THRESHOLD --------------------
    if (!isJoyLeftActive)
    {
        if (joystickX > MAX_THRESHOLD)
        {
            isJoyLeftActive = true;
            buzzerBeep(100);
            if (modelListener) modelListener->onButtonLeft();
            lastPressTime = currentTime;
        }
    }
    else
    {
        // Reset khi X < MAX_THRESHOLD - RESET_OFFSET (2500)
        if (joystickX < (MAX_THRESHOLD - RESET_OFFSET))
        {
            isJoyLeftActive = false;
        }
    }

    // -------------------- RIGHT: X < MIN_THRESHOLD --------------------
    if (!isJoyRightActive)
    {
        if (joystickX < MIN_THRESHOLD)
        {
            isJoyRightActive = true;
            buzzerBeep(100);
            if (modelListener) modelListener->onButtonRight();
            lastPressTime = currentTime;
        }
    }
    else
    {
        // Reset khi X > MIN_THRESHOLD + RESET_OFFSET (1500)
        if (joystickX > (MIN_THRESHOLD + RESET_OFFSET))
        {
            isJoyRightActive = false;
        }
    }
}

/**
 * ==============================================================================
 * READ BUTTONS GPIO - Đọc trạng thái nút bấm digital
 * ==============================================================================
 * Return: Bitmask
 *   - PE2-PE5: bit = 1 nếu HIGH (không nhấn), bit = 0 nếu LOW (nhấn)
 *   - PE6:     bit = 1 nếu HIGH (không nhấn), bit = 0 nếu LOW (nhấn)
 *   - PA0:     bit = 1 nếu HIGH (nhấn), bit = 0 nếu LOW (không nhấn)
 */
uint8_t Model::readButtonsGPIO()
{
    uint8_t state = 0;

    // PE2 - UP (Pull-up: HIGH = not pressed)
    if (HAL_GPIO_ReadPin(UP_GPIO_Port, UP_Pin) == GPIO_PIN_SET)
        state |= BTN_UP_MASK;

    // PE3 - DOWN
    if (HAL_GPIO_ReadPin(DOWN_GPIO_Port, DOWN_Pin) == GPIO_PIN_SET)
        state |= BTN_DOWN_MASK;

    // PE4 - LEFT
    if (HAL_GPIO_ReadPin(LEFT_GPIO_Port, LEFT_Pin) == GPIO_PIN_SET)
        state |= BTN_LEFT_MASK;

    // PE5 - RIGHT
    if (HAL_GPIO_ReadPin(RIGHT_GPIO_Port, RIGHT_Pin) == GPIO_PIN_SET)
        state |= BTN_RIGHT_MASK;

    // PE6 - JOY_BTN (Pull-up: HIGH = not pressed)
    if (HAL_GPIO_ReadPin(JOY_SW_GPIO_Port, JOY_SW_Pin) == GPIO_PIN_SET)
        state |= BTN_JOY_BTN_MASK;

    // PA0 - BACK (Active-HIGH: HIGH = pressed)
    if (HAL_GPIO_ReadPin(BTN_BACK_PORT, BTN_BACK_PIN) == GPIO_PIN_SET)
        state |= BTN_BACK_MASK;

    return state;
}

/**
 * ==============================================================================
 * READ JOYSTICK X - ADC1 Channel 5 (PA5)
 * ==============================================================================
 */
uint16_t Model::readJoystickX()
{
    uint16_t adcValue = 2048;  // Default center value
    
    HAL_ADC_Start(&hadc1);
    if (HAL_ADC_PollForConversion(&hadc1, 10) == HAL_OK)
    {
        adcValue = HAL_ADC_GetValue(&hadc1);
    }
    HAL_ADC_Stop(&hadc1);
    
    return adcValue;
}

/**
 * ==============================================================================
 * READ JOYSTICK Y - ADC3 Channel 4 (PF6)
 * ==============================================================================
 */
uint16_t Model::readJoystickY()
{
    uint16_t adcValue = 2048;  // Default center value
    
    HAL_ADC_Start(&hadc3);
    if (HAL_ADC_PollForConversion(&hadc3, 10) == HAL_OK)
    {
        adcValue = HAL_ADC_GetValue(&hadc3);
    }
    HAL_ADC_Stop(&hadc3);
    
    return adcValue;
}

/**
 * ==============================================================================
 * BUZZER CONTROL - Non-blocking beep
 * ==============================================================================
 */
void Model::buzzerBeep(uint32_t durationMs)
{
    buzzerOn();
    buzzerActive = true;
    buzzerStartTime = HAL_GetTick();
    buzzerDuration = durationMs;
}

void Model::processBuzzer()
{
    if (buzzerActive)
    {
        if (HAL_GetTick() - buzzerStartTime >= buzzerDuration)
        {
            buzzerOff();
            buzzerActive = false;
        }
    }
}

void Model::buzzerOn()
{
    HAL_GPIO_WritePin(BUZZER_GPIO_Port, BUZZER_Pin, GPIO_PIN_SET);
}

void Model::buzzerOff()
{
    HAL_GPIO_WritePin(BUZZER_GPIO_Port, BUZZER_Pin, GPIO_PIN_RESET);
}
