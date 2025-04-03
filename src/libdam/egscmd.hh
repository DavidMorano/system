/* egscmd HEADER */
/* encoding=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* entropy-gathering-server operations */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-12-01, David A­D­ Morano
	This module was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Name:
	egscmd

	Description:
	Commands to the Entropy Gathering Daemon (EGD):

+ 0x00 (get entropy level)
  0xMM (msb) 0xmm 0xll 0xLL (lsb)
+ 0x01 (read entropy nonblocking) 0xNN (bytes requested)
  0xMM (bytes granted) MM bytes
+ 0x02 (read entropy blocking) 0xNN (bytes desired)
  [block] NN bytes
+ 0x03 (write entropy) 0xMM 0xLL (bits of entropy) 0xNN (bytes of data) NN bytes
+ 0x04 (report PID)
  0xMM (length of PID string, not null-terminated) MM chars

*******************************************************************************/

#ifndef	EGSCMD_INCLUDE
#define	EGSCMD_INCLUDE
#ifdef	__cplusplus


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>
#include	<usyscalls.h>


enum struct egscmd {
	getlevel,
	readshort,
	read,
	write,
	getpid,
	overlast
} ; /* end enum-struct (egscmd) */


#endif /* __cplusplus */
#endif /* EGSCMD_INCLUDE */


