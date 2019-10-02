#include "clutter_coefficient.h"
const float toDegree = 180.0 / M_PI;


/** calculateClutterCoefficient
 * DESCRIPTION:
 *      Calculates the incremental clutter coefficient (also called the
 *		scattering coefficient).
 *		This function uses the interim clutter model based on the book:
 * 		Low-Angle Radar Land Clutter Measurements and Empirical Models
 *		J. Barrie Billingsley
 *		ISBN: 0852962304
 *		Chapter 4, page 295
 * ARGUMENTS:
 *      double lat, lon
 *          The lattitude and longitude of the origin in degrees.
 *      double radius
 *          The radius from the origin to populate in meters.
 */
float calculateClutterCoefficient(TerrainType terrain, float grazingAngle) {
	if (terrain == TerrainRural) {
		if (grazingAngle * toDegree > 4.0)
			return rand_float_weibull(2.6, dbToGain(-25));
		else if ( grazingAngle * toDegree > 1.5)
			return rand_float_weibull(2.7, dbToGain(-27));
		else if ( grazingAngle * toDegree > 0.75)
			return rand_float_weibull(3.0, dbToGain(-30));
		else if ( grazingAngle * toDegree > 0.25)
			return rand_float_weibull(3.5, dbToGain(-32));
		else if ( grazingAngle * toDegree > 0)
			return rand_float_weibull(3.8, dbToGain(-33));
		else if ( grazingAngle * toDegree > -0.25)
			return rand_float_weibull(3.4, dbToGain(-31));
		else if ( grazingAngle * toDegree > -0.75)
			return rand_float_weibull(3.3, dbToGain(-27));
		else
			return rand_float_weibull(2.3, dbToGain(-26));
	}
	else if (terrain == TerrainForest) {
		if (grazingAngle * toDegree > 1.0)
			return rand_float_weibull(2.0, dbToGain(-15));
		else if ( grazingAngle * toDegree > 0.3)
			return rand_float_weibull(2.7, dbToGain(-30));
		else if ( grazingAngle * toDegree > 0.0)
			return rand_float_weibull(2.0, dbToGain(-45));
		else if ( grazingAngle * toDegree > -0.25)
			return rand_float_weibull(3.4, dbToGain(-31));
		else if ( grazingAngle * toDegree > -0.75)
			return rand_float_weibull(3.3, dbToGain(-27));
		else
			return rand_float_weibull(2.3, dbToGain(-26));
	}
	else if (terrain == TerrainFarmland) {
		if (grazingAngle * toDegree > 1.5)
			return rand_float_weibull(2.4, dbToGain(-30));
		else if ( grazingAngle * toDegree > 0.75)
			return rand_float_weibull(4.0, dbToGain(-30));
		else if ( grazingAngle * toDegree > 0.4)
			return rand_float_weibull(5.4, dbToGain(-51));
		else if ( grazingAngle * toDegree > -0.25)
			return rand_float_weibull(3.4, dbToGain(-31));
		else if ( grazingAngle * toDegree > -0.75)
			return rand_float_weibull(3.3, dbToGain(-27));
		else
			return rand_float_weibull(2.3, dbToGain(-26));
	}
	else {
		if (grazingAngle * toDegree > 0.75)
			return rand_float_weibull(2.0, dbToGain(-38));
		else if ( grazingAngle * toDegree > 0.25)
			return rand_float_weibull(2.7, dbToGain(-56));
		else if ( grazingAngle * toDegree > 0.0)
			return rand_float_weibull(3.8, dbToGain(-68));
		else if ( grazingAngle * toDegree > -0.25)
			return rand_float_weibull(3.4, dbToGain(-31));
		else if ( grazingAngle * toDegree > -0.75)
			return rand_float_weibull(3.3, dbToGain(-27));
		else
			return rand_float_weibull(2.3, dbToGain(-26));
	} 
}