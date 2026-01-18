#ifndef MAINSCREENPRESENTER_HPP
#define MAINSCREENPRESENTER_HPP

#include <gui/model/ModelListener.hpp>
#include <mvp/Presenter.hpp>

using namespace touchgfx;

class MainScreenView;

/**
 * @class MainScreenPresenter
 * @brief Presenter trong kiến trúc MVP cho MainScreen (Game 4x4)
 *
 * Presenter đóng vai trò trung gian:
 * - Nhận events từ Model (thông qua ModelListener interface)
 * - Xử lý logic và gọi View để cập nhật UI
 *
 * Luồng dữ liệu: Model -> Presenter -> View
 */
class MainScreenPresenter : public touchgfx::Presenter, public ModelListener
{
public:
    MainScreenPresenter(MainScreenView& v);

    /**
     * @brief Được gọi tự động khi screen được activate
     * Khởi tạo logic có thể đặt ở đây
     */
    virtual void activate();

    /**
     * @brief Được gọi tự động khi screen được deactivate
     * Dọn dẹp có thể đặt ở đây
     */
    virtual void deactivate();

    virtual ~MainScreenPresenter() {}

    /**
     * @brief Chuyển sang màn hình Game Over
     */
    void gotoGameOverScreen();

    // ==============================================================================
    // Override ModelListener virtual functions
    // Nhận sự kiện button từ Model và forward đến View
    // ==============================================================================

    /**
     * @brief Xử lý khi nhấn nút UP (PE2)
     * Gọi View để di chuyển tiles lên trên
     */
    virtual void onButtonUp() override;

    /**
     * @brief Xử lý khi nhấn nút DOWN (PE3)
     * Gọi View để di chuyển tiles xuống dưới
     */
    virtual void onButtonDown() override;

    /**
     * @brief Xử lý khi nhấn nút LEFT (PE4)
     * Gọi View để di chuyển tiles sang trái
     */
    virtual void onButtonLeft() override;

    /**
     * @brief Xử lý khi nhấn nút RIGHT (PE5)
     * Gọi View để di chuyển tiles sang phải
     */
    virtual void onButtonRight() override;

    /**
     * @brief Xử lý khi nhấn nút BACK (PA0)
     * Gọi View để quay về màn hình chọn game mode
     */
    virtual void onButtonBack() override;

private:
    MainScreenPresenter();

    MainScreenView& view;
};

#endif // MAINSCREENPRESENTER_HPP
