/* dv10 SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */



uint div10u(uint u) noex {
    return (n * 0x00CD) >> 11 ;
} /* end subroutine (div10u) */



/* Effective 'q = n / 10' for 32-bit unsigned integers */
unsigned int divide_by_10(unsigned int n) {
    unsigned int q;
    q = (n >> 1) + (n >> 2);  // q = 0.75n
    q = q + (q >> 4);         // q = 0.796n
    q = q + (q >> 8);         // q = 0.799n
    q = q + (q >> 16);        // q = 0.8n (in fixed point)
    q = q >> 3;               // Divide by 8 to get n/10
    return q;
} /* end subroutine (divide_by_10) */


