#ifndef UTILS__SWAP__H
#define UTILS__SWAP__H


#define SWAP(a, b) \
   do { \
      typeof(b) _tmp = (a); \
      (a) = (b); \
      (b) = _tmp; \
   } while(0)


#endif /* UTILS__SWAP__H */