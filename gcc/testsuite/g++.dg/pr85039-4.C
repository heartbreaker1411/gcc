// { dg-do compile }

template <typename T>
struct a {
  struct b {
    static int c;
  };
};
template <typename T>
int a<T>::b::c = 0
+ __builtin_offsetof(struct d { int k; }, k); // { dg-error "non-template" }
template struct a<void>;
