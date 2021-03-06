#ifdef DEBUG_SHADOWING
#include <stdio.h>
#endif

#include "dem_parser/dem_parser.h"
#include <vector>
#include <assert.h>
#include <math.h>
#include <stdlib.h>
#include <thread>
using std::vector;

void ElevationMap::calculateShadowing() {
    std::thread* threads = new std::thread[threadCount];
    float mapDelta = float(mapSizeX-1)/float(threadCount);
    threads[0] = std::thread(   &ElevationMap::calculateShadowingPartial,
                                this,
                                0, 
                                int(mapDelta)
                            );
                            
    for (int i = 1; i < threadCount; i++)
        threads[i] = std::thread(   &ElevationMap::calculateShadowingPartial,
                                    this,
                                    int(mapDelta*i)+1,
                                    int(mapDelta*(i+1))
                                );
    for (int i = 0; i < threadCount; i++)
        threads[i].join();
}

void ElevationMap::calculateShadowingPartial(int start, int end) {
    for (int i = start; i <= end; i++) {
        calculateShadowingAlongLine(i, 0);
        calculateShadowingAlongLine(i, mapSizeY-1);
        calculateShadowingAlongLine(0, i);
        calculateShadowingAlongLine(mapSizeX-1, i);
    }
}

void ElevationMap::calculateShadowingAlongLine(int x1, int y1) {
    int x0 = mapOriginX;
    int y0 = mapOriginY;
    vector<chunk_t*> list;
    int deltax = x1 - x0;
    int deltay = y1 - y0;
   
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
            // Check if the chunk is out of range.
            if (map[x0][y0].shadowed & (0x01 << 1))
                break;
        } 
    }
    
    
    int end = list.size() - 1;
	for (int i = 0; i <= end; i++) {
        // Find max between 0 and end.
		int max_i = end;
		for (int k = 0; k <= end; k++) 
			if (list[k]->el > list[max_i]->el)
				max_i = k;

        double max_el = list[max_i]->el;
        // Shadow all chunks from max_i to end.
		for (int j = max_i+1; j <= end; j++) {
			if ((max_el - 5 * 3.141592 / 180.0) > list[j]->el)
                list[j]->shadowed |= (0x01);
        }
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
