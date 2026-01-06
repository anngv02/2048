#ifndef BEST_HPP
#define BEST_HPP

#include <gui_generated/containers/BestBase.hpp>

class Best : public BestBase
{
public:
    Best();
    virtual ~Best() {}

    virtual void initialize() override;
    void setScore(uint32_t best); 
protected:
    Unicode::UnicodeChar bestBuffer[10];
};

#endif // BEST_HPP
