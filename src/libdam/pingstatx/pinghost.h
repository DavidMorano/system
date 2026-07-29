/* pinghost HEADER */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* a little object to hold ping-host names */
/* version %I% last-modified %G% */


#ifndef	PINGHOST_INCLUDE
#define	PINGHOST_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */


#define	PINGHOST		struct pinghost


struct pinghost {
	cchar		*name ;
	int		intminping ;
	int		to ;
} ; /* end struct */

EXTERNC_begin

extern int pinghost_start	(pinghost *,cchar *,int,int,int) noex ;
extern int pinghost_finish	(pinghost *) noex ;

EXTERNC_end


#endif /* PINGHOST_INCLUDE */


