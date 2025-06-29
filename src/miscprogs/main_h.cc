/* program to clear the terminal screen */

#include	<cstdio>

#define		ESC	(char) 0x1B

int main() {
	printf("%1c[H",ESC) ;
	return 0 ;
}

