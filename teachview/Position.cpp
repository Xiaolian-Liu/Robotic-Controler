#include "Position.h"

Position::Position()
{
    for(int i=0; i<6; i++){
        inc[i] = 0;
    }
}

Position::~Position()
{

}

void Position::update(const int32_t * data)
{
    for(int i=0; i<6; i++)
    {
        inc[i] = *(data+i);
    }
}

int32_t Position::incpos(int index) const
{
    return inc[index];
}