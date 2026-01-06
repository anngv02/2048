#include <gui/containers/Best.hpp>

Best::Best()
{

}

void Best::initialize()
{
    BestBase::initialize();
    bestText.setWildcard(bestBuffer);
}
void Best::setScore(uint32_t best)
{
    Unicode::snprintf(bestBuffer, sizeof(bestBuffer), "%u", best);
    bestText.invalidate();  // Cập nhật lại hiển thị
}