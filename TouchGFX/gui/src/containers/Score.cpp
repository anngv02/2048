#include <gui/containers/Score.hpp>

Score::Score()
{

}

void Score::initialize()
{
    ScoreBase::initialize();
    // Gắn buffer cho TextArea
    scoreText.setWildcard(scoreBuffer);
}
// Hàm để cập nhật điểm
void Score::setScore(uint32_t score)
{
    Unicode::snprintf(scoreBuffer, sizeof(scoreBuffer), "%u", score);
    scoreText.invalidate();  // Yêu cầu cập nhật lại giao diện
}