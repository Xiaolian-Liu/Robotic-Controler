#ifndef POSITION_H
#define POSITION_H

#include <QMetaType>

class Position
{
private:
    int32_t inc[6];
public:
    Position();
    ~Position();
    void update(const int32_t *);
    int32_t incpos(int index) const;
};

Q_DECLARE_METATYPE(Position);

#endif