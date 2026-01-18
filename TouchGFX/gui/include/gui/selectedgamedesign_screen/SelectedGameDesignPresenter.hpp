#ifndef SELECTEDGAMEDESIGNPRESENTER_HPP
#define SELECTEDGAMEDESIGNPRESENTER_HPP

#include <gui/model/ModelListener.hpp>
#include <mvp/Presenter.hpp>

using namespace touchgfx;

class SelectedGameDesignView;

/**
 * @class SelectedGameDesignPresenter
 * @brief Presenter cho màn hình SelectedGameDesign (Menu chọn kích thước game)
 *
 * Điều khiển:
 * - UP (PE2): Di chuyển lên trong menu
 * - DOWN (PE3): Di chuyển xuống trong menu
 * - BACK/SELECT (PA0): Chọn mode hiện tại
 */
class SelectedGameDesignPresenter : public touchgfx::Presenter, public ModelListener
{
public:
    SelectedGameDesignPresenter(SelectedGameDesignView& v);

    virtual void activate();
    virtual void deactivate();

    virtual ~SelectedGameDesignPresenter() {}

    // ==============================================================================
    // Override ModelListener virtual functions
    // UP/DOWN: Di chuyển trong menu, BACK (PA0): Chọn mode
    // ==============================================================================
    virtual void onButtonUp() override;
    virtual void onButtonDown() override;
    virtual void onButtonBack() override;  // PA0 dùng để SELECT mode

private:
    SelectedGameDesignPresenter();

    SelectedGameDesignView& view;
};

#endif // SELECTEDGAMEDESIGNPRESENTER_HPP
