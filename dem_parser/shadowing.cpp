#ifdef DEBUG_SHADOWING
#include <stdio.h>
#endif

#include "dem_parser.h"
#include <vector>
#include <assert.h>
#include <math.h>
#include <stdlib.h>
using std::vector;

void ElevationMap::calculateShadowing() {
    for (int x = 0; x < mapSizeX; x++) {
        calculateShadowingAlongLine(mapOriginX, mapOriginY, x, mapSizeY - 1);
        calculateShadowingAlongLine(mapOriginX, mapOriginY, x, 0);
    }

    for (int y = 0; y < mapSizeY; y++) {
        calculateShadowingAlongLine(mapOriginX, mapOriginY, mapSizeX-1, y);
        calculateShadowingAlongLine(mapOriginX, mapOriginY, 0, y);
    } 
}

void ElevationMap::calculateShadowingAlongLine(int x0, int y0, int x1, int y1) {
    vector<chunk_t*> list;
    int deltax = x1 - x0;
    int deltay = y1 - y0;
   
    assert(!(deltax == 0 && deltay == 0));
    
    // Lines lies only on the Y axis.
    if (deltax == 0) {
        for (int y = y0; y != y1; y += (deltay > 0 ? 1 : -1))
            list.push_back(&map[x0][y]);
        list.push_back(&map[x1][y1]);
    }
    // Line lies only on the X axis.
    else if (deltay == 0) {
        for (int x = x0; x != x1; x += (deltax > 0 ? 1 : -1))
            list.push_back(&map[x][y0]);
        list.push_back(&map[x1][y1]);
    }
    // Otherwise, use Bresenham's line algorithm
    else {
        int dx = abs(x1-x0);
        int sx = x0<x1 ? 1 : -1;
        int dy = abs(y1-y0);
        int sy = y0<y1 ? 1 : -1; 
        int err = (dx>dy ? dx : -dy)/2, e2;
 
        for(;;) {
            list.push_back(&map[x0][y0]);
            if (x0==x1 && y0==y1) break;
            e2 = err;
            if (e2 >-dx) { err -= dy; x0 += sx; }
            if (e2 < dy) { err += dx; y0 += sy; }
        } 
    }
        
    // Calculate shadowed tiles along the line.
    for (int i = 0; i < list.size(); i++) {
        for (int j = list.size() - 1; j > i; j--) {
            if (list[i]->el - 5 * M_PI / 180.0 > list[j]->el)
                list[j]->shadowed = 1;
        }
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
