/* mheader HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* determine if a given c-string contains a mail-message header key */
/* version %I% last-modified %G% */


#ifndef	MHEADER_INCLUDE
#define	MHEADER_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>
#include	<usysbase.h>


EXTERNC_begin

extern int	mheader(cchar *,cchar *) noex ;

EXTERNC_end


#endif /* MHEADER_INCLUDE */


