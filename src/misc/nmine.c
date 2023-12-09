/* program to change the owner of a file */

/*
	David A.D. Morano
	July 1988
*/

/* Copyright � 1988 David A�D� Morano.  All rights reserved. */

/*******************************************************************************

        This program will change the owner of a file to the current UID. The
        current user needs write permission to the directory in order for the
        effect to take place.


*******************************************************************************/


#include	<sys/stat.h>
#include	<fcntl.h>
#include	<time.h>
#include	<stdio.h>

#include	"sys_types.h"
#include	"bfile.h"
#include	"localmisc.h"


#define		LINESIZE	4000


	extern int	bopen(), bclose() ;
	extern int	bread(), bwrite() ;
	extern int	breadln(), bprintf() ;



int main(argc,argv)
int	argc ;
char	*argv[] ;
{
	bfile	in, out, err ;

	int	i, rs, len ;

	char	buf[LINESIZE] ;


	rs = bopen(&err,BERR,"w",0666) ;
	if (rs < 0) return (rs) ;


	switch (argc) {

case 1:
	rs = bopen(&in,BIN,"r",0666) ;
	if (rs < 0) return (rs) ;

	rs = bopen(&out,BOUT,"w",0666) ;
	if (rs < 0) return (rs) ;

	break ;

case 2:
	rs = bopen(&in,argv[1],"r",0666) ;
	if (rs < 0) return (rs) ;

	rs = bopen(&out,BOUT,"w",0666) ;
	if (rs < 0) return (rs) ;

	break ;

case 3:
default:
	rs = bopen(&in,argv[1],"r",0666) ;
	if (rs < 0) return (rs) ;

	rs = bopen(&out,argv[2],"wc",0666) ;
	if (rs < 0) return (rs) ;

	break ;

	} ; /* end switch */



/* copy to the temporary file */

	while (1) {

		len = bread(&in,buf,LINESIZE) ;
		if (len <= 0) break ;

		rs = bwrite(&out,buf,len) ;

		if (rs < 0) {

			bprintf(efp,"bad return after write to out file %d\n",
				rs) ; 

			bclose(efp) ;

			return BAD ;
		}
	}


	bclose(&in) ;

	bclose(&out) ;

	bclose(&err) ;

	return OK ;
}


