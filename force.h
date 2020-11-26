#ifndef FORCE_H
#define FORCE_H

#include <QString>

class force{
public:
    force(double force_x, double force_y, double force_z, double moment_x,  double moment_y, double moment_z,  double position_x);
    force(double force_x, double force_y, double force_z,  double position_x);
    force();
    ~force();
    void addForces(force A);
    QString toString();
    double getForce_x();
    double getForce_y();
    double getForce_z();
    double getMoment_x();
    double getMoment_y();
    double getMoment_z();
    double getPos_x();
    double resulForce() ; //resultierende Kraft
    bool   lessThan(force B);

private:
    double force_x;
    double force_y;
    double force_z;
    double moment_x;
    double moment_y;
    double moment_z;
    double position_x;

};
#endif // FORCE_H



