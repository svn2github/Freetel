/* used to prototype algorithm for 3 digit count down timer used in wispcar.asm */

#include <stdio.h>
#include <stdlib.h>

int main() {
    int x,y,z;

    x = 1;
    y = 2;
    z = 3;

    while(1) {
	if (z == 0) {
	    if (y == 0) {
		if (x == 0) {
		    exit(0);
		}
		else {
		    x--;
		    y = 9;
		    z = 9;
		}
	    }
	    else {
		y--;
		z = 9;
	    }
	}
	else {
	    z--;
	}
	printf("%d%d%d\n",x,y,z);
    }
			    
}

