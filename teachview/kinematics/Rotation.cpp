#include "Rotation.h"
#include "kinematics.h"
Rotation::Rotation(double a, double b, double c, char mode)
{
    if('e' == mode)
    {
        Vector3d eul;
        eul << a, b, c;
        R = eul2ro(eul, 'd');
    }
    if('r' == mode)
    {
        Vector3d rpy;
        rpy << a, b, c;
        R = rpy2ro(rpy, 'd');
    }
}
//Rotation::Rotation(Vector3d axis, double angle)
//{

//}
Rotation::Rotation(Matrix3d m)
{
    R = m;
}

Rotation::~Rotation()
{

}

