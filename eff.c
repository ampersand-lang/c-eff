// necessary imports
#include <stdlib.h>
#include <stdio.h>

// this is the declaration of the extern `yield` function that is defined in `yield.asm`
__attribute__((noreturn))
extern void yield(void *);

// we use the `Handler` struct before it's defined, so we have to typedef it
typedef struct Handler Handler;

// just a function type typedef
// here it's specialized to accept and return an int, but those could be anything in &
typedef int (*HandlerCallback)(Handler *, int);

// the actual handler struct
typedef struct Handler {
  // the callback to which we wanna call in the case of an effect
  HandlerCallback callback;
  // if we yield, this is the address to which we wanna yield
  void *yield_to;
  // and if we yield, put the result into this pointer
  void *yield_result;
} Handler;

// a specialized version of `std::optional` from C++ for ints
typedef struct Option {
  int has;
  int value;
} Option;

// this is the effectful function we wanna call
int effectful_add1(Handler *on_err, int a) {
  // this is the error case
  // this would overflow
  if (a == (int)((1l << 31) - 1)) {
    // and this is the effect handler invokation
    // we pass the handler itself and the error-ful value
    return on_err->callback(on_err, a);
  }
  // this won't overflow
  return a + 1;
}

// the sensible default value for integers
int handle_zero(Handler *self, int a) {
  (void) self;
  (void) a;
  return 0;
}

// in &, this would be automatically generated and inlined
void wrap_effectful_add1(Handler *on_err, Option *result, int a) {
  // we call effectful_add1 normally
  int value = effectful_add1(on_err, a);
  // and set an optional type to its `Some` variant
  Option opt = { .has = 1, .value = value };
  // and return it normally
  *result = opt;
}

// this is the handler that yields the `None` value
// this would be automatically generated and inlined in &
int handle_yield_none(Handler *self, int a) {
  (void) a;
  // set the yield result to None
  ((Option *) self->yield_result)->has = 0;
  // and finally, yield
  yield(self->yield_to);
  // unreachable, because `yield` is a magical function that returns
  //  to a different point in the call stack
}

// program entry point
int main() {
  // a normal application of effectful_add1 with the default-to-zero handler
  Handler _tmp1 = { .callback = handle_zero, .yield_to = NULL, .yield_result = NULL };
  int x = effectful_add1(&_tmp1, 1);

  // print x to prove it works
  printf("%d\n", x);

  // a normal application of effectful_add1 with the yield-none handler
  Option opt_y;
  Handler _tmp2 = { .callback = handle_yield_none, .yield_to = &&L1, .yield_result = &opt_y };
  // this wrapper would be inlined in &
  wrap_effectful_add1(&_tmp2, &opt_y, 1);

  // the first instruction after the call to effectful_add1
  // this is where we want to yield to
L1:

  // print y to prove that it works
  if (opt_y.has) {
    printf("Some(%d)\n", opt_y.value);
  } else {
    printf("None\n");
  }

  // a normal application of effectful_add1 with the yield-none handler
  Option opt_z;
  Handler _tmp3 = { .callback = handle_yield_none, .yield_to = &&L2, .yield_result = &opt_z };
  // this wrapper would be inlined in &
  wrap_effectful_add1(&_tmp3, &opt_z, (1l << 31) - 1);

  // the first instruction after the call to effectful_add1
  // this is where we want to yield to
L2:

  // print z to prove that it works
  if (opt_z.has) {
    printf("Some(%d)\n", opt_z.value);
  } else {
    printf("None\n");
  }

  return 0;
}
