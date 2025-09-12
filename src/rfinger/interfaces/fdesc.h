/* fdesc HEADER */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */


#ifndef	FDESC_INCLUDE
#define	FDESC_INCLUDE


#define	FDESC		struct fdesc_head
#define	FDESC_A		struct fdesc_args


struct fdesc_args {
	int		fd ;
	int		pid ;
	int		flags ;
} ;

struct fdesc_head {
	uint		magic ;
	int		fd ;
	int		pid ;
	int		flags ;
} ;

typedef	FDESC		fdesc ;
typedef	FDESC_A		fdesc_a ;


#endif /* FDESC_INCLUDE */


