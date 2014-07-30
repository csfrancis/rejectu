#include <ruby.h>
#include <ruby/encoding.h>
#ifdef __SSE2__
#include <emmintrin.h>
#endif

static VALUE mRejectu = Qnil;
static VALUE idEncoding, idTo_s;

#ifdef __SSE2__
static inline int
has_utf8_supplementary_planes(__m128i v)
{
  v = _mm_srli_epi16(v, 4);
  v = _mm_cmpeq_epi16(v, _mm_set1_epi16(0x0f));
  return _mm_movemask_epi8(v) == 0 ? 0 : 1;
}
#endif

static inline void
validate_utf8_input(VALUE str)
{
  VALUE encoding;

  Check_Type(str, T_STRING);

  encoding = rb_funcall(rb_funcall(str, idEncoding, 0), idTo_s, 0);
  if (TYPE(encoding) != T_STRING || strcmp(RSTRING_PTR(encoding), "UTF-8") != 0) {
    rb_raise(rb_eArgError, "input string is not UTF-8");
  }
}

static VALUE
is_valid(VALUE self, VALUE str)
{
  unsigned char *p, *end;
  long len, remain;
#ifdef __SSE2__
  __m128i chunk, part;
  int mask;
#endif

  validate_utf8_input(str);

  len = RSTRING_LEN(str);
  p = RSTRING_PTR(str);
  end = RSTRING_END(str);

#ifdef __SSE2__
  /* advance p until it's 16 byte aligned */
  while (((uintptr_t) p & 0xf) != 0 && p < end) {
    if ((*p & 0xf0) == 0xf0) {
      return Qfalse;
    }
    p++;
  }

  while (p < end) {
    if (end - p < 16)
      break;

    chunk = _mm_load_si128((__m128i *) p);
    /* check if the top bit of any of the bytes is set, which is 1 if the character is multibyte */
    mask = _mm_movemask_epi8(chunk);
    if (mask) {
      /*
       * If there's a multi-byte character somewhere in this chunk, we need to check if it's a codepoint
       * from the supplementary plane (11110xxx 10xxxxxx 10xxxxxx 10xxxxxx).
       *
       * 1) Unpack the chunk into two halves (16-bit integers)
       * 2) Shift each 16-bit integer 4 bits to the right
       * 3) Check if the value is 0xf (first four bits set to 1)
       * 4) Check the high bit of each 8-bit integer
       *
       * If the result of step 4 is non-zero, the part has a supplementary plane character.
       *
       * Example: the string "hello test! \xf0\x9f\x98\x80" (13 characters, 16 bytes)
       *
       * UTF-8 representation:
       * h        e        l        l        o        <space>  t        e
       * 01101000 01100001 01101100 01101100 01101111 00100000 01110100 01100001
       *
       * s        t        !        <space>  😀 GRINNING FACE (1F600)
       * 01110011 01110100 00100001 00100000 11110000 10011111 10011000 10000000
       *
       * Low part:
       *
       * 1) Compare the low part into 16 bit values   = 0x00680065006c006c006f002000740065
       * 2) Shift each 16 bit value to the right by 4 = 0x00060006000600060006000000070006
       * 3) Compare each 16 bit value to 0xf          = 0x00000000000000000000000000000000
       * 4) Check the high bit of each 8-bit value    = 0
       *
       * No supplementary plane characters in this part
       *
       * High part:
       *
       * 1) Compare the low part into 16 bit values   = 0x007300740021002000f0009f00980080
       * 2) Shift each 16 bit value to the right by 4 = 0x0007000700020002000f000900090008
       * 3) Compare each 16 bit value to 0xf          = 0x0000000000000000ffff000000000000
       * 4) Check the high bit of each 8-bit value    = 0xc0 (0b0000000011000000)
       *
       * The result is non-zero, so this part has a supplementary plane character.
       *
       */
      if (has_utf8_supplementary_planes(_mm_unpacklo_epi8(chunk, _mm_setzero_si128())) ||
          has_utf8_supplementary_planes(_mm_unpackhi_epi8(chunk, _mm_setzero_si128()))) {
        return Qfalse;
      }
    }

    p += 16;
  }
#endif

  remain = end - p;
  while (remain) {
    if ((*p & 0xf0) == 0xf0) {
      return Qfalse;
    }
    p++;
    remain = end - p;
  }

  return Qtrue;
}

static VALUE
do_scrub(VALUE str)
{
  VALUE out_str;
  unsigned char *p, *end, *out_start, *out;
  long len, out_len;

  validate_utf8_input(str);

  len = RSTRING_LEN(str);
  p = RSTRING_PTR(str);
  end = RSTRING_END(str);

  out_start = out = (unsigned char *) malloc(len);
  if (!out_start) {
    rb_raise(rb_eNoMemError, "out of memory");
  }

  while (p < end) {
    if ((*p & 0xf0) == 0xf0) {
      if ((*p & 0xfc) == 0xfc) {
        p += 6;
      } else if ((*p & 0xf8) == 0xf8) {
        p += 5;
      } else {
        p += 4;
      }
      *out++ = '?';
    } else {
      *out++ = *p++;
    }
  }
  *out = '\0';

  out_str = rb_enc_str_new(out_start, out - out_start, rb_utf8_encoding());
  free(out_start);

  return out_str;
}

static VALUE
scrub(VALUE self, VALUE str)
{
  if (is_valid(self, str) == Qtrue) {
    return rb_enc_str_new(RSTRING_PTR(str), RSTRING_LEN(str), rb_utf8_encoding());
  }
  return do_scrub(str);
}

static VALUE
scrub_bang(VALUE self, VALUE str)
{
  VALUE repl;
  if (is_valid(self, str) == Qtrue) {
    return str;
  }
  repl = do_scrub(str);
  if (!NIL_P(repl)) rb_str_replace(str, repl);
  return str;
}

void
Init_rejectu()
{
  mRejectu = rb_define_module("Rejectu");

  rb_define_singleton_method(mRejectu, "valid?", is_valid, 1);
  rb_define_singleton_method(mRejectu, "scrub", scrub, 1);
  rb_define_singleton_method(mRejectu, "scrub!", scrub_bang, 1);

  idEncoding = rb_intern("encoding");
  idTo_s = rb_intern("to_s");
}
