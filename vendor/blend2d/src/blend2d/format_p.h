// This file is part of Blend2D project <https://blend2d.com>
//
// See blend2d.h or LICENSE.md for license and copyright information
// SPDX-License-Identifier: Zlib

#ifndef BLEND2D_FORMAT_P_H_INCLUDED
#define BLEND2D_FORMAT_P_H_INCLUDED

#include "format.h"
#include "tables_p.h"
#include "support/intops_p.h"

//! \cond INTERNAL
//! \addtogroup blend2d_internal
//! \{

//! Pixel formats used internally and never exposed to users.
enum BLFormatInternal : uint32_t {
  //! Internal pixel format that is the same as XRGB32, but the unused component
  //! is guaranteed to always be 0xFF so the format can be treated as PRGB32 by
  //! compositors.
  BL_FORMAT_FRGB32 = BL_FORMAT_MAX_VALUE + 1u,
  BL_FORMAT_ZERO32 = BL_FORMAT_MAX_VALUE + 2u,

  //! Count of internal pixel formats.
  BL_FORMAT_INTERNAL_COUNT = BL_FORMAT_MAX_VALUE + 3u
};

//! Pixel format flags used internally.
enum BLFormatFlagsInternal : uint32_t {
  BL_FORMAT_FLAG_FULL_ALPHA     = 0x10000000u,
  BL_FORMAT_FLAG_ZERO_ALPHA     = 0x20000000u,

  BL_FORMAT_ALL_FLAGS =
    BL_FORMAT_FLAG_RGB           |
    BL_FORMAT_FLAG_ALPHA         |
    BL_FORMAT_FLAG_RGBA          |
    BL_FORMAT_FLAG_LUM           |
    BL_FORMAT_FLAG_LUMA          |
    BL_FORMAT_FLAG_INDEXED       |
    BL_FORMAT_FLAG_PREMULTIPLIED |
    BL_FORMAT_FLAG_BYTE_SWAP     ,

  BL_FORMAT_COMPONENT_FLAGS =
    BL_FORMAT_FLAG_LUM           |
    BL_FORMAT_FLAG_RGB           |
    BL_FORMAT_FLAG_ALPHA
};

static_assert(int(BL_FORMAT_INTERNAL_COUNT) <= int(BL_FORMAT_RESERVED_COUNT),
              "Internal format count cannot overflow reserved format count");

static_assert(BL_FORMAT_COMPONENT_FLAGS == 0x7u,
              "Component flags of BLFormat must be at LSB");

static constexpr uint32_t blFormatFlagsStatic(uint32_t format) noexcept {
  return format == BL_FORMAT_PRGB32 ? BL_FORMAT_FLAG_RGBA           |
                                      BL_FORMAT_FLAG_PREMULTIPLIED  |
                                      BL_FORMAT_FLAG_BYTE_ALIGNED   :
         format == BL_FORMAT_XRGB32 ? BL_FORMAT_FLAG_RGB            |
                                      BL_FORMAT_FLAG_BYTE_ALIGNED   |
                                      BL_FORMAT_FLAG_UNDEFINED_BITS :
         format == BL_FORMAT_A8     ? BL_FORMAT_FLAG_ALPHA          |
                                      BL_FORMAT_FLAG_BYTE_ALIGNED   :
         format == BL_FORMAT_FRGB32 ? BL_FORMAT_FLAG_RGB            |
                                      BL_FORMAT_FLAG_BYTE_ALIGNED   |
                                      BL_FORMAT_FLAG_UNDEFINED_BITS |
                                      BL_FORMAT_FLAG_FULL_ALPHA     :
         format == BL_FORMAT_ZERO32 ? BL_FORMAT_FLAG_RGBA           |
                                      BL_FORMAT_FLAG_BYTE_ALIGNED   |
                                      BL_FORMAT_FLAG_UNDEFINED_BITS |
                                      BL_FORMAT_FLAG_ZERO_ALPHA     : 0;
}

static BL_INLINE bool blFormatInfoHasSameAlphaLayout(const BLFormatInfo& a, const BLFormatInfo& b) noexcept {
  return (a.sizes[3] == b.sizes[3]) & (a.shifts[3] == b.shifts[3]) ;
}

static BL_INLINE bool blFormatInfoHasSameRgbLayout(const BLFormatInfo& a, const BLFormatInfo& b) noexcept {
  return (a.sizes[0] == b.sizes[0]) & (a.shifts[0] == b.shifts[0]) &
         (a.sizes[1] == b.sizes[1]) & (a.shifts[1] == b.shifts[1]) &
         (a.sizes[2] == b.sizes[2]) & (a.shifts[2] == b.shifts[2]) ;
}

static BL_INLINE bool blFormatInfoHasSameRgbaLayout(const BLFormatInfo& a, const BLFormatInfo& b) noexcept {
  return (a.sizes[0] == b.sizes[0]) & (a.shifts[0] == b.shifts[0]) &
         (a.sizes[1] == b.sizes[1]) & (a.shifts[1] == b.shifts[1]) &
         (a.sizes[2] == b.sizes[2]) & (a.shifts[2] == b.shifts[2]) &
         (a.sizes[3] == b.sizes[3]) & (a.shifts[3] == b.shifts[3]) ;
}

//! Converts absolute masks like `0x3F0` to mask sizes and shift shifts as used
//! by `BLFormatInfo`. Only useful for pixel formats with absolute masks up to
//! 64 bits (uint64_t input). Commonly used to convert pixel formats that use
//! 32 or less bits.
template<typename T>
static void blFormatInfoAssignAbsoluteMasks(BLFormatInfo& info, const T* masks, size_t n = 4) noexcept {
  typedef typename std::make_unsigned<T>::type U;

  memset(info.sizes, 0, sizeof(info.sizes));
  memset(info.shifts, 0, sizeof(info.shifts));

  for (size_t i = 0; i < n; i++) {
    U m = U(masks[i]);
    if (!m)
      continue;

    uint32_t shift = BLIntOps::ctz(m);
    m >>= shift;
    uint32_t size = m >= 0xFFFFFFFF ? 32 : BLIntOps::ctz(~uint32_t(m));

    info.sizes[i] = uint8_t(size);
    info.shifts[i] = uint8_t(shift);
  }
}

//! \}
//! \endcond

#endif // BLEND2D_FORMAT_P_H_INCLUDED
