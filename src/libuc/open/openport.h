/* openport HEADER */
/* lang=C++20 */

/* subroutine to open a priviledged network port */
/* version %I% last-modified %G% */


/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	OPENPORT_INCLUDE
#define	OPENPORT_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<sockaddress.h>
#include	<openportmsg.h>
#include	<localmisc.h>


EXTERNC_begin

extern int openport(int,int,int,SOCKADDRESS *) ;

EXTERNC_end


#endif /* OPENPORT_INCLUDE */


