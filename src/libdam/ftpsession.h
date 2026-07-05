/* ftpsession HEADER */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* FTPSESSION operations */
/* version %I% last-modified %G% */


/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	FTPSESSION_INCLUDE
#define	FTPSESSION_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<FtpLibrary.h>


/* object defines */
#define	FTPSESSION		struct ftpsession_head


struct ftpsession_head {
	FTP		*sp ;
	uint		magval ;
} ; /* end struct */

typedef FTPSESSION	ftpsession ;

EXTERNC_begin

extern int ftpsession_open(FTPSESSION *,char *,char *,char *,char *) noex ;
extern int ftpsession_fileread(FTPSESSION *,char *) noex ;
extern int ftpsession_setmode(FTPSESSION *,int) noex ;
extern int ftpsession_read(FTPSESSION *,char *,int) noex ;
extern int ftpsession_write(FTPSESSION *,char *,int) noex ;
extern int ftpsession_close(FTPSESSION *) noex ;

EXTERNC_end


#endif /* FTPSESSION_INCLUDE */


