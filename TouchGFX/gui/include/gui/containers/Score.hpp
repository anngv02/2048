#ifndef SCORE_HPP
#define SCORE_HPP

#include <gui_generated/containers/ScoreBase.hpp>

class Score : public ScoreBase
{
public:
    Score();
    virtual ~Score() {}

    virtual void initialize();
    void setScore(uint32_t score);
protected:
    Unicode::UnicodeChar scoreBuffer[10]; 
};

#endif // SCORE_HPP
