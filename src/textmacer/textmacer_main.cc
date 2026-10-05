/* main (textmacer) */


#include	<envstandards.h>	/* ordered first to configure */

#include	<cstdio>



/* ARGSUSED */
int main(int argc,cchar **argv,cchar **envv)
{
	FILE		*ifp = stdin ;
	FILE		*ofp = stdout ;
	int		c ;

	while ((c = fgetc(ifp)) >= 0) {
		if (c == '\r') c = '\n' ;
		fputc(c,ofp) ;
	} /* end while */

	return 0 ;
}
/* end subroutine (main) */


