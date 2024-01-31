/* hash_hsieh SUPPORT */
/* lang=C20 */

/* this is the "super-fast" hash function by Paul Hsieh (2004-2008) */
/* version %I% last-modified %G% */

#include	<envstandards.h>	/* first to configure */
#include	<stdint.h>
#include	<utypedefs.h>
#include	<clanguage.h>


/* local defines */

#undef get16bits
#if (defined(__GNUC__) && defined(__i386__)) || defined(__WATCOMC__) \
  || defined(_MSC_VER) || defined (__BORLANDC__) || defined (__TURBOC__)
#define get16bits(d) (*((const uint16_t *) (d)))
#endif

#if !defined (get16bits)
#define get16bits(d) ((((uint32_t)(((const uint8_t *)(d))[1])) << 8)\
                       +(uint32_t)(((const uint8_t *)(d))[0]) )
#endif


/* local namespaces */


/* local typedefs */


/* external subroutines */


/* local varaibles */


/* exported variables */


/* exported subroutines */

uint hash_hsieh(cchar *data,int len) noex {
	uint	hash = 0 ;
	if ((len > 0) && data) {
	uint	tmp ;
	int	rem = (len & 3) ;
	hash = len ;
    len >>= 2;
    /* main loop */
    for (;len > 0; len--) {
        hash  += get16bits (data);
        tmp    = (get16bits (data+2) << 11) ^ hash;
        hash   = (hash << 16) ^ tmp;
        data  += 2*sizeof (uint16_t);
        hash  += hash >> 11;
    } /* end for */
    /* handle end cases */
    switch (rem) {
        case 3: hash += get16bits (data);
                hash ^= hash << 16;
                hash ^= ((signed char)data[sizeof (uint16_t)]) << 18;
                hash += hash >> 11;
                break;
        case 2: hash += get16bits (data);
                hash ^= hash << 11;
                hash += hash >> 17;
                break;
        case 1: hash += (signed char)*data;
                hash ^= hash << 10;
                hash += hash >> 1;
		break ;
    } /* end switch */
    /* force "avalanching" of final 127 bits */
    hash ^= hash << 3;
    hash += hash >> 5;
    hash ^= hash << 4;
    hash += hash >> 17;
    hash ^= hash << 25;
    hash += hash >> 6;
	} /* end if (valid) */
	return hash ;
}
/* end subroutine (hash_hsieh) */


