/* div10 SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */



uint div10u(uint num) noex {
    return (num * 0x00CD) >> 11 ;
} /* end subroutine (div10u) */



uint divide_by_10(uint num) {
    uint q ;
    q = (num >> 1) + (num >> 2) ;	/* q = 0.75n */
    q = q + (q >> 4) ;			/* q = 0.796n */
    q = q + (q >> 8) ;			/* q = 0.799n */
    q = q + (q >> 16) ;			/* q = 0.8n (in fixed point) */
    q = q >> 3 ;			/* divide by 8 to get n/10 */
    return q ;
} /* end subroutine (divide_by_10) */


#include <cstdint>

// Performs unsigned 64-bit division by 10 optimized for GCC 15.2
[[nodiscard]] constexpr uint64_t divide_by_10(uint64_t n) noexcept {
    // 1. Define the multiplier: ceil(2^64 / 10)
    constexpr uint64_t multiplier = 0xCCCC'CCCC'CCCC'CCCDULL;

    // 2. Compute the upper 64 bits of the 128-bit product using GCC extensions
    uint128_t product = static_cast<uint128_t>(n) * multiplier;

    // 3. Shift right by 64 bits to extract the high 64 bits
    uint64_t high_bits = static_cast<uint64_t>(product >> 64);

    // 4. Final logical right shift by 3
    return high_bits >> 3;
} /* end subruutine (divide_by_10) */


#include <cstdint>

struct DivMod32 Result {
    uint32_t quotient;
    uint32_t remainder;
};

// Computes both quotient and remainder for n / 10 using fixed-point math
[[nodiscard]] constexpr DivMod32Result divide_by_10_32bit(uint32_t n) noexcept {
    // 1. Define the multiplier: ceil(2^32 / 10)
    constexpr uint64_t multiplier = 0xCCCC'CCCCUL ;

    // 2. Compute the 64-bit product
    uint64_t product = static_cast<uint64_t>(n) * multiplier;
    
    // 3. Shift right by 32 bits to get the intermediate quotient, 
    // then logical right shift by 3 (totaling a shift of 35 bits)
    uint32_t quotient = static_cast<uint32_t>(product >> 35);

    // 4. Calculate the remainder using the formula: 
    // remainder = n - (quotient * 10)
    // This is significantly faster than using the modulo (%) operator
    uint32_t remainder = n - (quotient * 10);

    return {quotient, remainder};
} /* end subroutine (divide_by_10_32bit) */



