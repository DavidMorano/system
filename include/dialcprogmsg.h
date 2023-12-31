/* dialcprogmsg */

/* dial a program within the current machine cluster */


/* Copyright � 2003 David A�D� Morano.  All rights reserved. */

#ifndef	DIALCPROGMSG_INCLUDE
#define	DIALCPROGMSG_INCLUDE


#include	<envstandards.h>
#include	<sys/types.h>
#include	<sys/socket.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<clanguage.h>
#include	<sockaddress.h>
#include	<localmisc.h>


/* flags */
#define	DIALCPROGMSG_FERRCHAN	0x01

#define	DIALCPROGMSG_END	struct dialcprogmsg_end
#define	DIALCPROGMSG_LIGHT	struct dialcprogmsg_light


enum dialcprogmsgtypes {
	dialcprogmsgtype_end,
	dialcprogmsgtype_pwd,
	dialcprogmsgtype_fname,
	dialcprogmsgtype_arg,
	dialcprogmsgtype_env,
	dialcprogmsgtype_light,
	dialcprogmsgtype_nodename,
	dialcprogmsgtype_overlast
} ;

struct dialcprogmsg_end {
	uchar		type ;
	ushort		len ;
	ushort		flags ;
	int		opts ;
} ;

struct dialcprogmsg_light {
	uchar		type ;
	ushort		len ;
	ushort		salen1 ;
	ushort		salen2 ;
	SOCKADDRESS	saout ;
	SOCKADDRESS	saerr ;
} ;

#ifdef	__cplusplus
extern "C" {
#endif

extern int dialcprogmsg_end(char *,int,int,DIALCPROGMSG_END *) noex ;
extern int dialcprogmsg_light(char *,int,int,DIALCPROGMSG_LIGHT *) noex ;

#ifdef	__cplusplus
}
#endif


#endif /* DIALCPROGMSG_INCLUDE */


