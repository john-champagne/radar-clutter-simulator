#ifdef DEBUG_SHADOWING
#include <stdio.h>
#endif

#include "dem_parser/dem_parser.h"
#include <vector>
#include <assert.h>
#include <math.h>
#include <stdlib.h>
using std::vector;

void ElevationMap::calculateShadowing() {
    for (int x = 0; x < mapSizeX; x++) {
        calculateShadowingAlongLine(x, mapSizeY - 1);
        calculateShadowingAlongLine(x, 0);
    }

    for (int y = 0; y < mapSizeY; y++) {
        calculateShadowingAlongLine(mapSizeX-1, y);
        calculateShadowingAlongLine(0, y);
    } 
}

void ElevationMap::calculateShadowingAlongLine(int x, int y) {
    int x0 = mapOriginX;
    int y0 = mapOriginY;
    vector<chunk_t*> list;
    int deltax = x - x0;
    int deltay = y - y0;
   
    assert(!(deltax == 0 && deltay == 0));
    
    // Lines lies only on the Y axis.
    if (deltax == 0) {
        for (int y = y0; y != y; y += (deltay > 0 ? 1 : -1))
            list.push_back(&map[x0][y]);
        list.push_back(&map[x][y]);
    }
    // Line lies only on the X axis.
    else if (deltay == 0) {
        for (int x = x0; x != x; x += (deltax > 0 ? 1 : -1))
            list.push_back(&map[x][y0]);
        list.push_back(&map[x][y]);
    }
    // Otherwise, use Bresenham's line algorithm
    else {
        int dx = abs(x-x0);
        int sx = x0<x ? 1 : -1;
        int dy = abs(y-y0);
        int sy = y0<y ? 1 : -1; 
        int err = (dx>dy ? dx : -dy)/2, e2;
 
        for(;;) {
            list.push_back(&map[x0][y0]);
            if (x0==x && y0==y) break;
            e2 = err;
            if (e2 >-dx) { err -= dy; x0 += sx; }
            if (e2 < dy) { err += dx; y0 += sy; }
        } 
    }
        
    int end = list.size() - 1;
	for (int i = 0; i <= end; i++) {
        // Find max between 0 and end.
		int max_i = end;
		for (int k = 0; k <= end; k++) {
			if (list[k]->el > list[max_i]->el)
				max_i = k;
		}
        // Shadow all chunks from max_i to end.
		for (int j = max_i + 1; j <= end; j++)
			list[j]->shadowed = 1;
		end = max_i - 1;
	}
}

#ifdef DEBUG_SHADOWING

int main() {
    ElevationMap E;
    E.populateMap(38.52,-98.10,1000,10);
    for (int i = 0; i < E.mapSizeX; i++)
        for (int j = 0; j < E.mapSizeY; j++) {
            chunk_t m = E.getMap(i,j);
            printf("%d%c", m.shadowed, (j == (E.mapSizeY - 1)) ? '\n' : ',');
        }
}

#endif
