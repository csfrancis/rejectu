#include <ruby.h>

static VALUE mRejectu = Qnil;

static VALUE
is_valid(VALUE self, VALUE str)
{
  unsigned char *p, *end;
  long len, remain;

  Check_Type(str, T_STRING);
  len = RSTRING_LEN(str);
  p = RSTRING_PTR(str);
  end = RSTRING_END(str);

  while (p < end) {
    if (end - p < 8)
      break;

    if (((*((uint64_t *) p) & 0xf0f0f0f0f0f0f0f0) & 0x8080808080808080) != 0)
      return Qfalse;

    p += 8;
  }

  remain = end - p;
  while (remain) {
    if (((*((unsigned char *) p) & 0xf0) & 0x80) != 0)
      return Qfalse;
    p++;
    remain = end - p;
  }

  return Qtrue;
}

void
Init_rejectu()
{
  mRejectu = rb_define_module("Rejectu");

  rb_define_singleton_method(mRejectu, "valid?", is_valid, 1);
}
