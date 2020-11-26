#include "force.h"
#include <cmath>

force::force(double force_x1, double force_y2, double force_z3, double moment_x1,  double moment_y2, double moment_z3,  double position_x1){

    this->force_x = force_x1;
    this->force_y= force_y2;
    this->force_z= force_z3;
    this->moment_x= moment_x1;
    this->moment_y= moment_y2;
    this->moment_z= moment_z3;
    this->position_x=position_x1;

}
force::force(double force_x1, double force_y2, double force_z3,  double position_x1){

    this->force_x= force_x1;
    this->force_y= force_y2;
    this->force_z= force_z3;
    this->moment_x= 0;
    this->moment_y= 0;
    this->moment_z= 0;
    this->position_x=position_x1;

}
force::~force(){
    this->force_x= 0;
    this->force_y= 0;
    this->force_z= 0;
    this->moment_x= 0;
    this->moment_y= 0;
    this->moment_z= 0;
}
void force::addForces(force A){
    this->force_x += A.getForce_x();
    this->force_y += A.getForce_y();
    this->force_z += A.getForce_z();
    this->moment_x += A.getMoment_x();
    this->moment_y += A.getMoment_y();
    this->moment_z += A.getMoment_z();
}
QString force::toString(){          //Basic Output only Forces
    return ("FRes: "+QString::number(resulForce(),'f',1)+" N  with   Fx: "+QString::number(this->force_x,'f',1)+" N,   "+"Fy: "+QString::number(this->force_y,'f',1)+" N,   "+"Fz: "+QString::number(this->force_z,'f',1)+" N,   "+"PosX: "+QString::number(this->position_x)+" mm");
}
double force::resulForce(){
    return sqrt(this->force_x*this->force_x+this->force_y*this->force_y+this->force_z*this->force_z);
}
double force::getForce_x(){
    return this->force_x;
}
double force::getForce_y(){
    return this->force_y;
}
double force::getForce_z(){
    return this->force_z;
}
double force::getMoment_x(){
    return this->moment_x;
}
double force::getMoment_y(){
    return this->moment_y;
}
double force::getMoment_z(){
    return this->moment_z;
}
double force::getPos_x(){
    return this->position_x;
}
bool force::lessThan(force B){
    if(this->getPos_x() < B.getPos_x()){
        return true;
    }else{
        return false;
    }
}

;
