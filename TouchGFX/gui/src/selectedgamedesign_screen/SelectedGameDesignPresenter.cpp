#include <gui/selectedgamedesign_screen/SelectedGameDesignView.hpp>
#include <gui/selectedgamedesign_screen/SelectedGameDesignPresenter.hpp>

SelectedGameDesignPresenter::SelectedGameDesignPresenter(SelectedGameDesignView& v)
    : view(v)
{
}

void SelectedGameDesignPresenter::activate()
{
}

void SelectedGameDesignPresenter::deactivate()
{
}

// ==============================================================================
// Button Event Handlers - Nhận từ Model, forward đến View
// ==============================================================================

/**
 * @brief Xử lý nút UP (PE2)
 * Di chuyển lên trong menu chọn kích thước game
 */
void SelectedGameDesignPresenter::onButtonUp()
{
    view.onMoveUp();
}

/**
 * @brief Xử lý nút DOWN (PE3)
 * Di chuyển xuống trong menu chọn kích thước game
 */
void SelectedGameDesignPresenter::onButtonDown()
{
    view.onMoveDown();
}

/**
 * @brief Xử lý nút BACK (PA0)
 * Trong màn hình menu này, PA0 được dùng để SELECT mode
 */
void SelectedGameDesignPresenter::onButtonBack()
{
    view.onSelectMode();
}
