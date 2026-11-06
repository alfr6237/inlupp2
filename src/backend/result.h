
///@file result.h
///@brief interanl file that declares macros for better error handleing

#define RESULT(T, E)                                                           \
  struct {                                                                     \
    bool is_ok;                                                                \
    union {                                                                    \
      T value;                                                                 \
      E error;                                                                 \
    };                                                                         \
  }

#define RESULT_VOID(E)                                                         \
  struct {                                                                     \
    bool is_ok;                                                                \
    E error;                                                                   \
  }
