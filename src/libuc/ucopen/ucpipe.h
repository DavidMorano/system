/* ucpipe HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* get random data from the system */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-02-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	UCPIPE_INCLUDE
#define	UCPIPE_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>
#include	<usysbase.h>


EXTERNC_begin

extern int	uc_pipe(int *) noex ;
extern int	uc_pipes(int *,int) noex ;
extern int	uc_piper(int *,int,int) noex ;
local inline int uc_pipe2(int *pipes,int of) noex {
	return uc_pipes(pipes,of) ;
}

EXTERNC_end


#endif /* UCPIPE_INCLUDE */


