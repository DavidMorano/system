/* usysdefs_ioctl HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* these are (a multitude) of various UNIX® system pre-processor defines */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-02-09, David A-D- Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A-D- Morano.  All rights reserved. */

/*******************************************************************************

	Description:
	This file defines various preprocessor variables for use
	in setting up the UNIX®-system default for various buffer
	sizes.  The real run-time system buffer sizes are (indeed)
	determined at runtime where they may exist.

	Usage:
	Among the most important uses for the data contained herein
	is the BUFSIZE management facility.  It uses the data below
	to set default buffer sizes for those systems that do not
	have dynamicly managed buffer size support in the system
	kernel.

*******************************************************************************/

#ifndef	USYSDEFSIOCTL_INCLUDE
#define	USYSDEFSIOCTL_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<sys/ioctl.h>		/* POSIX® */


/* missing UNIX® signals */
enum missingioctls {
	missingioctl_synthetic = 1000,
	missingioctl_sendfd,
	missingioctl_recvfd,
	missingioctl_look,
	missingioctl_push,
	missingioctl_pop
} ; /* end enum */

#ifndef	I_SENDFD
#define	I_SENDFD	missingioctl_sendfd
#endif
#ifndef	I_RECVFD
#define	I_RECVFD	missingioctl_recvfd
#endif
#ifndef	I_LOOK
#define	I_LOOK		missingioctl_look
#endif
#ifndef	I_PUSH
#define	I_PUSH		missingioctl_push
#endif
#ifndef	I_POP
#define	I_POP		missingioctl_pop
#endif


#endif /* USYSDEFSIOCTL_INCLUDE */


