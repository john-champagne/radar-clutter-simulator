#include "clutter_coefficient.h"
#include "random.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

const float toDegree = 180.0 / M_PI;


/** calculateClutterCoefficient
 * DESCRIPTION:
 *      Calculates the incremental clutter coefficient (also called the
 *      scattering coefficient).
 *      This function uses the interim clutter model based on the book:
 *      Low-Angle Radar Land Clutter Measurements and Empirical Models
 *      J. Barrie Billingsley
 *      ISBN: 0852962304
 *      Chapter 4, page 295
 * ARGUMENTS:
 *      double lat, lon
 *          The lattitude and longitude of the origin in degrees.
 *      double radius
 *          The radius from the origin to populate in meters.
 */
float calculateClutterCoefficient(TerrainType terrain, float grazingAngle) {
	if (terrain == TerrainRural) {
		if (grazingAngle * toDegree > 4.0)
			return rand_float_weibull(2.6, dBToGain(-25));
		else if ( grazingAngle * toDegree > 1.5)
			return rand_float_weibull(2.7, dBToGain(-27));
		else if ( grazingAngle * toDegree > 0.75)
			return rand_float_weibull(3.0, dBToGain(-30));
		else if ( grazingAngle * toDegree > 0.25)
			return rand_float_weibull(3.5, dBToGain(-32));
		else if ( grazingAngle * toDegree > 0)
			return rand_float_weibull(3.8, dBToGain(-33));
		else if ( grazingAngle * toDegree > -0.25)
			return rand_float_weibull(3.4, dBToGain(-31));
		else if ( grazingAngle * toDegree > -0.75)
			return rand_float_weibull(3.3, dBToGain(-27));
		else
			return rand_float_weibull(2.3, dBToGain(-26));
	}
	else if (terrain == TerrainForest) {
		if (grazingAngle * toDegree > 1.0)
			return rand_float_weibull(2.0, dBToGain(-15));
		else if ( grazingAngle * toDegree > 0.3)
			return rand_float_weibull(2.7, dBToGain(-30));
		else if ( grazingAngle * toDegree > 0.0)
			return rand_float_weibull(2.0, dBToGain(-45));
		else if ( grazingAngle * toDegree > -0.25)
			return rand_float_weibull(3.4, dBToGain(-31));
		else if ( grazingAngle * toDegree > -0.75)
			return rand_float_weibull(3.3, dBToGain(-27));
		else
			return rand_float_weibull(2.3, dBToGain(-26));
	}
	else if (terrain == TerrainFarmland) {
		if (grazingAngle * toDegree > 1.5)
			return rand_float_weibull(2.4, dBToGain(-30));
		else if ( grazingAngle * toDegree > 0.75)
			return rand_float_weibull(4.0, dBToGain(-30));
		else if ( grazingAngle * toDegree > 0.4)
			return rand_float_weibull(5.4, dBToGain(-51));
		else if ( grazingAngle * toDegree > -0.25)
			return rand_float_weibull(3.4, dBToGain(-31));
		else if ( grazingAngle * toDegree > -0.75)
			return rand_float_weibull(3.3, dBToGain(-27));
		else
			return rand_float_weibull(2.3, dBToGain(-26));
	}
	else {
		if (grazingAngle * toDegree > 0.75)
			return rand_float_weibull(2.0, dBToGain(-38));
		else if ( grazingAngle * toDegree > 0.25)
			return rand_float_weibull(2.7, dBToGain(-56));
		else if ( grazingAngle * toDegree > 0.0)
			return rand_float_weibull(3.8, dBToGain(-68));
		else if ( grazingAngle * toDegree > -0.25)
			return rand_float_weibull(3.4, dBToGain(-31));
		else if ( grazingAngle * toDegree > -0.75)
			return rand_float_weibull(3.3, dBToGain(-27));
		else
			return rand_float_weibull(2.3, dBToGain(-26));
	} 
}
