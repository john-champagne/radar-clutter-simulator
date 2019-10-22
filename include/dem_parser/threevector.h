#ifndef THREEVECTOR_H
#define THREEVECTOR_H

#ifndef M_PI
    #define M_PI 3.14159265358979323846
#endif

class ThreeVector {
	public:
		ThreeVector();
        ThreeVector(double, double, double);
		double x, y, z;
		
        double magnitude();		
		ThreeVector normalize();
		
		static ThreeVector crossProduct(ThreeVector,ThreeVector);
		static double dotProduct(ThreeVector,ThreeVector);
		static double dotProduct(ThreeVector,double, double,double);
	    
        double dot(double,double,double);
        double dot(ThreeVector);
        ThreeVector cross(double, double, double);
        ThreeVector cross(ThreeVector);
	
		ThreeVector operator- (ThreeVector const & t) {
			ThreeVector i;
			i.x = x - t.x;
			i.y = y - t.y;
			i.z = z - t.z;
			return i;
		}
		
		ThreeVector operator+ (ThreeVector const & t) {
			ThreeVector i;
			i.x = x + t.x;
			i.y = y + t.y;
			i.z = z + t.z;
			return i;
		}
};

ThreeVector calculateECEF(double lat, double lon, double height);

#endif
