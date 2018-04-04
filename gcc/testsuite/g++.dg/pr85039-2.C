// { dg-do compile }

struct d {
  static d *b;
} * d::b(__builtin_offsetof(struct {
  int i;
  struct a {
    int c() { return .1f; }
  };
}, i));
