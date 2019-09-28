#ifdef DEBUG_THREEVECTOR

#include <iostream>
#include <assert.h>
#include <stdlib.h>
using namespace std;

#endif

#include "threevector.h"
#include <math.h>

#define sind(x) (sin(fmod((x),360) * M_PI / 180))
#define cosd(x) (cos(fmod((x),360) * M_PI / 180))

ThreeVector::ThreeVector() {
    this->x = 0;
    this->y = 0;
    this->z = 0;
}

ThreeVector::ThreeVector(double x, double y, double z){
	this->x = x;
	this->y = y;
	this->z = z;
}

double ThreeVector::magnitude(){
    return sqrt(x*x + y*y + z*z);
}

ThreeVector ThreeVector::normalize() {
    double m = this->magnitude();
	ThreeVector a;
    if (m != 0) {
        a.x = x/m;
        a.y = y/m;
        a.z = z/m;
    }
    
    return a;
}

double ThreeVector::dotProduct(ThreeVector a ,ThreeVector b) {
	return a.x*b.x + a.y*b.y + a.z*b.z;
}

double ThreeVector::dotProduct(ThreeVector a , double x, double y, double z) {
	return a.x*x + a.y*y + a.z*z;
}

double ThreeVector::dot(double x, double y, double z){
    return ThreeVector::dotProduct(*this, x,y,z);
}

double ThreeVector::dot(ThreeVector b) {
    return ThreeVector::dotProduct(*this, b);
}

ThreeVector ThreeVector::crossProduct(ThreeVector a ,ThreeVector b) {
	ThreeVector cp(0,0,0);
	cp.x = (a.y * b.z - a.z * b.y);
	cp.y = -1.0*(a.x * b.z - a.z * b.x);
	cp.z = (a.x * b.y - a.y * b.x);
    return cp;
}

ThreeVector ThreeVector::cross(double x, double y, double z) {
    ThreeVector b(x,y,z);
    return ThreeVector::crossProduct(*this, b);
}

ThreeVector ThreeVector::cross(ThreeVector b) {
    return ThreeVector::crossProduct(*this, b);
}

ThreeVector calculateECEF(double lat, double lon, double height) {
    const double a = 6378137.0;
    const double b = 6356752.314245;
    
    double N = a*a / sqrt(pow(a*cosd(lat),2) + pow(b*sind(lat),2));
	double x = (N + height)*cosd(lat)*cosd(lon);
	double y = (N + height)*cosd(lat)*sind(lon);
	double z = (pow(b/a, 2)*N + height)*sind(lat);

    ThreeVector r(x,y,z);
    return r;
}

#ifdef DEBUG_THREEVECTOR

int main() {
    
    for (int i = 0; i < 10000000; i++) {
        int h = rand()%3;
        int v = rand()%1000 - 500;

        ThreeVector a;
        a.x = h == 0 ? v : 0;
        a.y = h == 1 ? v : 0;
        a.z = h == 2 ? v : 0;
        
        assert(abs(a.magnitude() - abs(v)) < 0.00001);
        assert(a.magnitude() >= 0.0);
    }

    for (int i = 0; i < 10000000; i++) {
        ThreeVector a(rand()%1000 - 500, rand()%1000 - 500, rand()%1000 - 500);
        ThreeVector b(rand()%1000 - 500, rand()%1000 - 500, rand()%1000 - 500);
        assert((a.normalize().magnitude() < 1.0001 && 
                a.normalize().magnitude() > 0.9999) || 
                a.magnitude() == 0.0 ); 
        assert(abs(a.cross(b).dot(a)) < 0.00001);
        assert(abs(a.cross(b).dot(b)) < 0.00001);
        
        ThreeVector c = a-b;
        assert((a.x - b.x) == c.x);
        assert((a.y - b.y) == c.y);
        assert((a.z - b.z) == c.z);

        ThreeVector d = a+b;
        assert((a.x + b.x) == d.x);
        assert((a.y + b.y) == d.y);
        assert((a.z + b.z) == d.z);
    }
    
    ThreeVector northPole = calculateECEF(0, 0, 0);
    cout << northPole.x << "," << northPole.y << "," << northPole.z << endl;
}

#endif

