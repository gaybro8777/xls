# Standard Library

go/dslx-std

<!--*
# Document freshness: For more information, see go/fresh-source.
freshness: { owner: 'sameeragarwal' reviewed: '2021-07-15' }
*-->

This page documents the DSLX standard library.

# `std.x`

## `std::bounded_minus_1`

```dslx-snippet
pub fn bounded_minus_1<N: u32>(x: uN[N]) -> uN[N]
```

Returns the value of `x - 1` with saturation at `0`.

## `std::abs`

```dslx-snippet
pub fn abs<BITS: u32>(x: sN[BITS]) -> sN[BITS]
```

Returns the absolute value of `x` as a signed number.

## `std::is_pow2`

```dslx-snippet
pub fn is_pow2<N: u32>(x: uN[N]) -> bool
```

Returns true when x is a non-zero power-of-two.

## `std::?mul`

```dslx-snippet
pub fn umul<N: u32, M: u32, R: u32 = N + M>(x: uN[N], y: uN[M]) -> uN[R]
pub fn smul<N: u32, M: u32, R: u32 = N + M>(x: sN[N], y: sN[M]) -> sN[R]
```

Returns product of `x` (`N` bits) and `y` (`M` bits) as an `N+M` bit value.

## `std::iterative_div`

```dslx-snippet
pub fn iterative_div<N: u32, DN: u32 = N * u32:2>(x: uN[N], y: uN[N]) -> uN[N]
```

Calculate `x / y` one bit at a time. This is an alternative to using the
division operator '/' which may not synthesize nicely.

## `std::div_pow2`

```dslx-snippet
pub fn div_pow2<N: u32>(x: bits[N], y: bits[N]) -> bits[N]
```

Returns `x / y` where `y` must be a non-zero power-of-two.

## `std::mod_pow2`

```dslx-snippet
pub fn mod_pow2<N: u32>(x: bits[N], y: bits[N]) -> bits[N]
```

Returns `x % y` where `y` must be a non-zero power-of-two.

## `std::ceil_div`

```dslx-snippet
pub fn ceil_div<N: u32>(x: uN[N], y: uN[N]) -> uN[N]
```

Returns the ceiling of (x divided by y).

## `std::round_up_to_nearest`

```
pub fn round_up_to_nearest(x: u32, y: u32) -> u32
```

Returns `x` rounded up to the nearest multiple of `y`.

## `std::?pow`

```dslx-snippet
pub fn upow<N: u32>(x: uN[N], n: uN[N]) -> uN[N]
pub fn spow<N: u32>(x: sN[N], n: uN[N]) -> sN[N]
```

Performs integer exponentiation as in Hacker's Delight, Section 11-3. Only
nonnegative exponents are allowed, hence the uN parameter for spow.

## `std::clog2`

```dslx-snippet
pub fn clog2<N: u32>(x: bits[N]) -> bits[N]
```

Returns `ceiling(log2(x))`, with one exception: When `x = 0`, this function
differs from the true mathematical function: `clog2(0) = 0` where as
`ceil(log2(0)) = -infinity`

This function is frequently used to calculate the number of bits required to
represent `x` possibilities. With this interpretation, it is sensible to define
`clog2(0) = 0`.

Example: `clog2(7) = 3`.

## `std::?max`

```
pub fn smax<N: u32>(x: sN[N], y: sN[N]) -> sN[N]
pub fn umax<N: u32>(x: uN[N], y: uN[N]) -> uN[N]
```

Returns the maximum of two integers.

## `std::umin`

```dslx-snippet
pub fn umin<N: u32>(x: uN[N], y: uN[N]) -> uN[N]
```

Returns the minimum of two unsigned integers.

## `Signed comparison`

```dslx-snippet
pub fn sge<N: u32>(x: uN[N], y: uN[N]) -> bool
pub fn sgt<N: u32>(x: uN[N], y: uN[N]) -> bool
pub fn sle<N: u32>(x: uN[N], y: uN[N]) -> bool
pub fn slt<N: u32>(x: uN[N], y: uN[N]) -> bool
```

**Explicit signed comparison** helpers for working with unsigned values, can be
a bit more convenient and a bit more explicit intent than doing casting of left
hand side and right hand side.

## `std::find_index`

```dslx-snippet
pub fn find_index<BITS: u32, ELEMS: u32>( array: uN[BITS][ELEMS], x: uN[BITS]) -> (bool, u32)
```

Returns (`found`, `index`) given array and the element to find within the array.

Note that when `found` is false, the `index` is `0` -- `0` is provided instead
of a value like `-1` to prevent out-of-bounds accesses from occurring if the
index is used in a match expression (which will eagerly evaluate all of its
arms), to prevent it from creating an error at simulation time if the value is
ultimately discarded from the unselected match arm.

## `std::lsb`

```dslx-snippet
pub fn lsb<N: u32>(x: uN[N]) -> u1
```

Extracts the LSB (Least Significant Bit) from the value `x` and returns it.

## `std::convert_to_bits`

```
pub fn convert_to_bits<N: u32>(x: bool[N]) -> uN[N]
```

Converts an array of `N` bools to a `bits[N]` value.

## `std::mask_bits`

```dslx-snippet
pub fn mask_bits<X: u32>() -> bits[X]
```

Returns a value with X bits set (of type bits[X]).

## `std::concat3`

```dslx-snippet
pub fn concat3<X: u32, Y: u32, Z: u32, R: u32 = X + Y + Z>(x: bits[X], y: bits[Y], z: bits[Z]) -> bits[R]
```

Concatenates 3 values of arbitrary bitwidths to a single value.

## `std::rrot`

```
pub fn rrot<N: u32>(x: bits[N], y: bits[N]) -> bits[N]
```

Rotate `x` right by `y` bits.