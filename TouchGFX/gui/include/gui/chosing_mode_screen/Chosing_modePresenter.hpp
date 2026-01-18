#ifndef CHOSING_MODEPRESENTER_HPP
#define CHOSING_MODEPRESENTER_HPP

#include <gui/model/ModelListener.hpp>
#include <mvp/Presenter.hpp>

using namespace touchgfx;

class Chosing_modeView;

/**
 * @class Chosing_modePresenter
 * @brief Presenter cho màn hình Chosing_mode (Menu chọn chế độ game)
 */
class Chosing_modePresenter : public touchgfx::Presenter, public ModelListener
{
public:
    Chosing_modePresenter(Chosing_modeView& v);

    virtual void activate();
    virtual void deactivate();

    virtual ~Chosing_modePresenter() {}

    // ==============================================================================
    // Override ModelListener virtual functions
    // UP/DOWN: Di chuyển trong menu, BACK (PA0): Chọn mode
    // ==============================================================================
    virtual void onButtonUp() override;
    virtual void onButtonDown() override;
    virtual void onButtonBack() override;  // PA0 dùng để SELECT mode

private:
    Chosing_modePresenter();

    Chosing_modeView& view;
};

#endif // CHOSING_MODEPRESENTER_HPP
