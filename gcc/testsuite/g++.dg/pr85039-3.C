// { dg-do compile }

// Check that the type defined in the initializer of a member of a
// completed class is defined remains visible in the enclosing
// namespace scope.

struct a {
  struct b {
    static int c;
  };
};
int a::b::c = 0
+ __builtin_offsetof(struct d { int k; }, k);
d e;
a::b::d f;
