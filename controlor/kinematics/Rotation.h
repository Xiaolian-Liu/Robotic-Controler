#ifndef ROTATION_H
#define ROTATION_H
#include <Eigen/Dense>
using Eigen::Matrix3d;
using Eigen::Vector3d;

class Rotation
{
public:
    Rotation(double a, double b, double c, char mode);
//    Rotation(Vector3d axis, double angle);
    Rotation(Matrix3d m);
    ~Rotation();
private:
    Matrix3d R;
};

#endif // ROTATION_H
