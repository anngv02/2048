#include <gui/mainscreen_screen/MainScreenView.hpp>
#include <gui/mainscreen_screen/MainScreenPresenter.hpp>

/**
 * @brief Constructor - bind reference đến View
 * @param v Reference đến MainScreenView
 */
MainScreenPresenter::MainScreenPresenter(MainScreenView& v)
    : view(v)
{
}

/**
 * @brief Được gọi khi screen activate
 * Có thể thêm initialization logic ở đây nếu cần
 */
void MainScreenPresenter::activate()
{
    // Initialization khi screen được bật
}

/**
 * @brief Được gọi khi screen deactivate
 * Có thể thêm cleanup logic ở đây nếu cần
 */
void MainScreenPresenter::deactivate()
{
    // Cleanup khi screen tắt
}

/**
 * @brief Chuyển sang màn hình Game Over
 */
void MainScreenPresenter::gotoGameOverScreen()
{
    static_cast<FrontendApplication*>(Application::getInstance())->gotoGameOverScreenScreenSlideTransitionEast();
}

// ==============================================================================
// Button Event Handlers - Nhận từ Model, forward đến View
// ==============================================================================

/**
 * @brief Xử lý nút UP
 * Gọi View để thực hiện di chuyển tiles lên trên
 */
void MainScreenPresenter::onButtonUp()
{
    view.onMoveUp();
}

/**
 * @brief Xử lý nút DOWN
 * Gọi View để thực hiện di chuyển tiles xuống dưới
 */
void MainScreenPresenter::onButtonDown()
{
    view.onMoveDown();
}

/**
 * @brief Xử lý nút LEFT
 * Gọi View để thực hiện di chuyển tiles sang trái
 */
void MainScreenPresenter::onButtonLeft()
{
    view.onMoveLeft();
}

/**
 * @brief Xử lý nút RIGHT
 * Gọi View để thực hiện di chuyển tiles sang phải
 */
void MainScreenPresenter::onButtonRight()
{
    view.onMoveRight();
}

/**
 * @brief Xử lý nút BACK
 * Gọi View để quay về màn hình chọn game mode
 */
void MainScreenPresenter::onButtonBack()
{
    view.onNavigateBack();
}
