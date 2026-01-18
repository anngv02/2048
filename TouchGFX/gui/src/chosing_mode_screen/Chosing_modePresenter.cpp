#include <gui/chosing_mode_screen/Chosing_modeView.hpp>
#include <gui/chosing_mode_screen/Chosing_modePresenter.hpp>

Chosing_modePresenter::Chosing_modePresenter(Chosing_modeView& v)
    : view(v)
{
}

void Chosing_modePresenter::activate()
{
}

void Chosing_modePresenter::deactivate()
{
}

// ==============================================================================
// Button Event Handlers - Nhận từ Model, forward đến View
// ==============================================================================

/**
 * @brief Xử lý nút UP (PE2)
 * Di chuyển lên trong menu chọn mode
 */
void Chosing_modePresenter::onButtonUp()
{
    view.onMoveUp();
}

/**
 * @brief Xử lý nút DOWN (PE3)
 * Di chuyển xuống trong menu chọn mode
 */
void Chosing_modePresenter::onButtonDown()
{
    view.onMoveDown();
}

/**
 * @brief Xử lý nút BACK (PA0)
 * Trong màn hình menu này, PA0 được dùng để SELECT mode
 */
void Chosing_modePresenter::onButtonBack()
{
    view.onSelectMode();
}
