/* headadds HEADER */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* address type headers */
/* version %I% last-modified %G% */


/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	HEADADDS_INCLUDE
#define	HEADADDS_INCLUDE


enum headadds {
	headadd_to,
	headadd_cc,
	headadd_bcc,
	headadd_from,
	headadd_sender,
	headadd_replyto,
	headadd_overlast
} /* end enum (headadds) */

#define	A_TO		headadd_to
#define	A_CC		headadd_cc
#define	A_BCC		headadd_bcc
#define	A_FROM		headadd_from
#define	A_SENDER	headadd_sender
#define	A_REPLYTO	headadd_replyto
#define	A_OVERLAST	headadd_overlast

#define	NADDS		headadd_overlast


#endif /* HEADADDS_INCLUDE */


