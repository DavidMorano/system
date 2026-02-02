/* datestr HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* PCS user-account-database file management */
/* version %I% last-modified %G% */


#ifndef	DATESTR_INCLUDE
#define	DATESTR_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>
#include	<usysbase.h>


EXTERNC_begin

extern char *datestr_envelope(time_t,char *) noex ;
extern char *datestr_header(time_t,char *) noex ;

EXTERNC_end


#endif /* DATESTR_INCLUDE */


