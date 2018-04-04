// { dg-do compile { target c++14 } }

constexpr int a() {
 return
  __builtin_offsetof(struct {
    int i;
    short b {
      __builtin_offsetof(struct {
	int j;
        struct c {
          void d() {
          }
        };
      }, j)
    };
  }, i);
}
