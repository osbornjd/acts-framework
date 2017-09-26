//
//  parallel_for.h
//  ACTFW
//
//  Created by Hadrien Grasland on 11/05/16.
//

/// @file

#ifndef ACTFW_CONCURRENCY_PARALLEL_FOR_H
#define ACTFW_CONCURRENCY_PARALLEL_FOR_H 1

#include <exception>
#include "ACTFW/Framework/ProcessCode.hpp"

/// We will use this simple helper around the _Pragma operator of C99
/// and C++11, if it is not defined elsewhere
///
#ifndef ACTFW_PRAGMA
#define ACTFW_PRAGMA(x) _Pragma(#x)
#endif

/// Out-of-order multithreaded equivalent of the following serial loop
/// (macro arguments capitalized for emphasis):
///
///   for(size_t INDEX = START; index < END; ++index) {
///      ...
///   }
///
/// One difference with a serial loop is that you cannot easily abort
/// the computation using break and return statements. This is a
/// limitation of the OpenMP 3.1 tasking backend that we currently use.
///
/// Replacements for the common use cases of break and return are
/// provided in ACTFW_PARALLEL_FOR_BREAK and ACTFW_PARALLEL_FOR_ABORT.
///
/// Unlike raw OpenMP 3.1, we also support exception propagation.
///
/// One difference with a serial loop is that code which is passed as
/// an argument to this macro should contain no preprocessor directive.
///
/// @param index must be unique within the enclosing scope.
///
#define ACTFW_PARALLEL_FOR(index, start, end, ...)                  \
  {                                                                 \
    bool               actfw_parallel_break_##index = false;        \
    bool               actfw_parallel_abort_##index = false;        \
    std::exception_ptr actfw_parallel_exptr_##index;                \
                                                                    \
    ACTFW_PRAGMA(omp parallel for)                                  \
    for(size_t index = (start); index < (end); ++index)             \
    {                                                               \
      try {                                                         \
        ACTFW_PRAGMA(omp flush (actfw_parallel_break_##index))      \
        if(actfw_parallel_break_##index) continue;                  \
        __VA_ARGS__                                                 \
      } catch(...) {                                                \
        actfw_parallel_exptr_##index = std::current_exception();    \
        ACTFW_PARALLEL_FOR_BREAK(index)                             \
      }                                                             \
    }                                                               \
                                                                    \
    if(actfw_parallel_abort_##index) return ProcessCode::ABORT;     \
    if(actfw_parallel_exptr_##index) {                              \
      std::rethrow_exception(actfw_parallel_exptr_##index);         \
    }                                                               \
  }

/// Abort an enclosing ACTFW_PARALLEL_FOR construct and return to the
/// enclosing scope.
///
/// @param index must match the index parameter given for that loop.
///
#define ACTFW_PARALLEL_FOR_BREAK(index)                             \
  {                                                                 \
    actfw_parallel_break_##index = true;                            \
    ACTFW_PRAGMA(omp flush (actfw_parallel_break_##index))          \
    continue;                                                       \
  }

/// Abort an enclosing ACTFW_PARALLEL_FOR and exit the host function by
/// returning ProcessCode::ABORT.
///
/// @param index must match the index parameter given for that loop.
///
#define ACTFW_PARALLEL_FOR_ABORT(index)                             \
  {                                                                 \
    actfw_parallel_abort_##index = true;                            \
    ACTFW_PARALLEL_FOR_BREAK(index)                                 \
  }

// TODO: Once we can assume OpenMP 4.0 support from the host compiler,
//       break out of the loop using #pragma omp cancel

#endif  // ACTFW_CONCURRENCY_PARALLEL_FOR_H
