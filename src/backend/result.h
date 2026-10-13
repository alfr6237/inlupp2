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

// Kan vara lite mer nice en ett enda enum då vi nu kan switcha på ett error och
// veta att det inte är succes tex
