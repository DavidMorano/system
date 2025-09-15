/* lu HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* lookup a reference in the databases */
/* version %I% last-modified %G% */


/* revision history:

	= 2000-05-14, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 2000 David A­D­ Morano.  All rights reserved. */

#ifndef	LU_INCLUDE
#define	LU_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>
#include	<bfile.h>
#include	<hdb.h>


#define	LU		struct lu_head
#define	LU_FL		struct lu_flags
#define	LU_CUR		struct hdb_cursor
#define	LU_VAL		struct hdb_value


struct lu_flags {
	uint		open:1 ;
} ;

struct lu_value {
	bfile		infile ;
	bfile		outfile ;
	bfile		errfile ;
	LU_FL		fl ;
} ;

struct lu_head {
	hdb		lh ;
	uint		magic ;
} ;

typedef LU		lu ;
typedef LU_FL		lu_fl ;
typedef LU_CUR		lu_cur ;
typedef LU_VAL		lu_val ;

EXTERNC_begin

extern int	lu_init(lu *) noex ;
extern int	lu_free(lu *) noex ;
extern int	lu_count(lu *) noex ;
extern int	lu_getkey(lu *,lu_cur *,cchar **) noex ;
extern int	lu_delcur(lu *,lu_cur *) noex ;

EXTERNC_end


#endif /* LU_INCLUDE */


