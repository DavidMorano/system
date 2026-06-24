/* progaudit SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* audito an index database */
/* version %I% last-modified %G% */

#define	CF_DEBUG 	0		/* run-time debug print-outs */

/* revision history:

	= 1999-03-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1999 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Name:
	progaudit

	Description:
	This subroutine processes a single file.

	Synopsis:
	int progaudit(pip,aip,terms,dbname,outfname)

	Arguments:
	struct proginfo	*pip ;
	struct arginfo	*aip ;
	const uchar	terms[] ;
	const uchar	dbname[] ;
	char		outfname[] ;

	Arguments:
	- pip		program information pointer

	Returns:
	>=0		OK
	<0		error code

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<sys/types.h>		/* POSIX */
#include	<sys/param.h>		/* POSIX */
#include	<sys/stat.h>		/* POSIX */
#include	<sys/mman.h>		/* POSIX */
#include	<unistd.h>		/* POSIX */
#include	<climits>		/* CSTD */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<cstring>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<baops.h>		/* LIBU */
#include	<bfile.h>		/* LIBUC */
#include	<vecstr.h>		/* LIBUC */
#include	<field.h>		/* LIBUC */
#include	<eigendb.h>		/* LIBUC */
#include	<naturalwords.h>	/* LIBDAM */
#include	<rtags.h>		/* LIBDAM */
#include	<localmisc.h>		/* LIBU */
#include	<libdebug.h>		/* LIBDEBUG |DEBUGPRINTF(3debug)| */

#include	"offindex.h"
#include	"config.h"
#include	"defs.h"


/* local defines */

#ifndef	KEYBUFLEN
#define	KEYBUFLEN	NATURALWORDLEN
#endif

#ifndef	TAGBUFLEN
#define	TAGBUFLEN	MIN((MAXPATHLEN + 40),(2 * 1024))
#endif

#ifndef	LOWBUFLEN
#define	LOWBUFLEN	NATURALWORDLEN
#endif


/* external subroutines */

extern int	hashmapverify(struct proginfo *,struct hashmap *) ;


/* external variables */


/* local structures */


/* forward references */

static int	tagindex(bfile *,HDB *) ;
static int	procquery(struct proginfo *,struct hashmap *,
			const uchar *,vecstr *,vecstr *,bfile *,bfile *) ;
static int	tag_parse(RTAGS_TAG *,cchar *,cchar *,
			cchar *,int) ;


/* local variables */

static const uchar	aterms[] = {
	0x00, 0x3E, 0x00, 0x00,
	0x09, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00
} ; /* end array */


/* exported variables */


/* exported subroutines */

int progaudit(PI *pip,AI *aip,cchar *terms,cc *dbname,char *outfname) noex {
	int	rs ;
	DEBUGPRINTF("progaudit: dbname=%s\n",dbname) ;
	{
	rs = auditdb(pip,dbname) ;
	}
	return rs ;
} /* end subroutine (progaudit) */


