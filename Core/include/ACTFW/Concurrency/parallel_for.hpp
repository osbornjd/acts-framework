// This file is part of the ACTS project.
//
// Copyright (C) 2017 ACTS project team
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

//
//  parallel_for.h
//  ACTFW
//
//  Created by Hadrien Grasland on 11/05/16.
//

/// @file

#ifndef ACTFW_CONCURRENCY_PARALLEL_FOR_H
#define ACTFW_CONCURRENCY_PARALLEL_FOR_H 1

#include <boost/optional.hpp>
#include <boost/variant/get.hpp>
#include <boost/variant/variant.hpp>
#include <exception>
#include "ACTFW/Framework/ProcessCode.hpp"

// Let's keep as much as possible inside of namespaces
namespace FW {
namespace Details {

  // A C++ for loop iteration may either run normally, break, continue, return,
  // or throw. Scenarios which do not emit data can be expressed via an enum...
  enum class LoopFlow { NormalIteration, Continue, Break };

  // ...from which one can derive a Boost variant which covers the complete set
  // of look iteration outcomes, including early returns and exceptions.
  template <typename ReturnT>
  using LoopOutcome = boost::variant<LoopFlow, ReturnT, std::exception_ptr>;

  // Tell whether a certain loop iteration outcome requires exiting the loop
  template <typename T>
  bool
  loop_outcome_is_fatal(const LoopOutcome<T>& outcome)
  {
    // Any outcome which is not representable by LoopFlow is fatal
    if (outcome.which() != 0) return true;

    // Among the outcomes representable by LoopFlow, only Break is fatal
    return (outcome == LoopOutcome<T>(LoopFlow::Break));
  }

// This macro wraps a user-provided for loop iteration code into a functor
// which returns a LoopOutcome. It handles all loop iteration outcomes which
// can only be handled by a macro, that is, everything but exceptions.
//
// The RETURN_TYPE parameter indicates the return type of the function inside
// which the underlying for loop is located, and INDEX is the name of the loop
// variable expected by the user's iteration code.
//
// Ideally, RETURN_TYPE should be inferred instead of caller-provider, but
// that does not seem possible in C++14. I think C++17's flavor of auto is
// powerful enough for such inference, so maybe something for the future...
//
#define _ACTFW_WRAPPED_LOOP_ITERATION(RETURN_TYPE, INDEX, ...)                 \
  /* To produce a functor in a random context, we use a catch-all lambda */    \
  [&](size_t INDEX) -> FW::Details::LoopOutcome<RETURN_TYPE> {                 \
    /* We set up a fake for loop environment to catch all user actions */      \
    for (int _dummy_##INDEX = 0; _dummy_##INDEX < 2; ++_dummy_##INDEX) {       \
      if (_dummy_##INDEX == 0) {                                               \
        /* The user's loop iteration code is pasted here. It may return */     \
        /* or throw an exception, that is handled in higher-level code */      \
        __VA_ARGS__                                                            \
                                                                               \
        /* If control reaches this point, the loop iteration code */           \
        /* finished normally without continuing, breaking, or returning */     \
        return FW::Details::LoopFlow::NormalIteration;                         \
      } else {                                                                 \
        /* If control reaches this point, the loop iteration was skipped */    \
        /* using the "continue" control flow keyword */                        \
        return FW::Details::LoopFlow::Continue;                                \
      }                                                                        \
    }                                                                          \
                                                                               \
    /* It control reaches this point, the loop was aborted using the */        \
    /* "break" control flow keyword */                                         \
    return FW::Details::LoopFlow::Break;                                       \
  }

  // Thanks to the loop iteration wrapper above, most of the ACTSFW parallel
  // for loop can now be written as real C++ code, instead of a macro.
  //
  // This function runs a parallel for loop, with loop iterations going from
  // "start" to "end", and a per-iteration behavior specified by a functor
  // taking the current parallel loop index as a parameter, and returning a loop
  // iteration outcome. That functor is expected to be generated via the
  // previously defined _ACTFW_WRAPPED_LOOP_ITERATION macro.
  //
  // If there was an early return from the loop, this function propagates it up.
  // Otherwise, it returns an empty boost::optional.
  //
  template <typename T>
  boost::optional<T>
  parallel_for_impl(size_t                                start,
                    size_t                                end,
                    std::function<LoopOutcome<T>(size_t)> iteration)
  {
    // These control variables are used to tell OpenMP to exit the parallel loop
    // early if needed, and to record why we had to do it.
    //
    // TODO: Once we can assume good OpenMP 4.0 support from the host compiler,
    //       break out of the loop more efficiently using #pragma omp cancel
    //
    bool           exit_loop_early = false;
    LoopOutcome<T> exit_reason     = LoopFlow::NormalIteration;

// Our parallel for loop is implemented using OpenMP
#pragma omp parallel for
    for (size_t index = start; index < end; ++index) {
// Skip remaining loop iterations if asked to exit the loop early
#pragma omp flush(exit_loop_early)
      if (exit_loop_early) continue;

      // Run this loop iteration and record the outcome, exceptions included
      LoopOutcome<T> outcome = LoopFlow::NormalIteration;
      try {
        outcome = iteration(index);
      } catch (...) {
        outcome = std::current_exception();
      }

      // Abort the loop if the iteration's outcome states that we should do so
      if (loop_outcome_is_fatal(outcome)) {
#pragma omp critical
        {
          exit_reason     = std::move(outcome);
          exit_loop_early = true;
#pragma omp flush(exit_loop_early)
        }
      }
    }

    // Analyze the loop termination cause and react accordingly
    switch (exit_reason.which()) {
    // The loop exited normally or via break, no need to do anything
    case 0:
      return boost::optional<T>();

    // The loop was exited due to an early return, propagate it up the stack
    case 1:
      return boost::get<T>(std::move(exit_reason));

    // The loop was exited because an exception was thrown. Rethrow it.
    case 2:
      auto exception = boost::get<std::exception_ptr>(std::move(exit_reason));
      std::rethrow_exception(std::move(exception));
    }
  }

/// The following macro is the out-of-order multithreaded equivalent of the
/// following serial loop (macro arguments capitalized for emphasis):
///
///   for(size_t INDEX = START; index < END; ++index) {
///      ...
///   }
///
/// Unlike raw OpenMP 3.1, we also support breaks, early returns, and
/// exception propagation, to allow for more idiomatic C++ code. On the other
/// hand, due to the way the C preprocessor handles macro arguments, the loop
/// iteration code should not contain any preprocessor directive.
///
/// Due to limitations of C++14's type inference, this macro may currently
/// only be called in a function which returns FW::ProcessCode. This
/// restriction may be lifted in the future if C++ type inference improves.
///
#define ACTFW_PARALLEL_FOR(INDEX, START, END, ...)                             \
  /* This dummy do-while asserts that the macro's output is a statement */     \
  do {                                                                         \
    /* Execute the parallel for loop */                                        \
    auto optional_early_return                                                 \
        = FW::Details::parallel_for_impl<FW::ProcessCode>(                     \
            (START),                                                           \
            (END),                                                             \
            _ACTFW_WRAPPED_LOOP_ITERATION(                                     \
                FW::ProcessCode, INDEX, __VA_ARGS__));                         \
                                                                               \
    /* Return early from the host function if asked to do so */                \
    if (optional_early_return) {                                               \
      return *optional_early_return;                                           \
    }                                                                          \
  } while (false);
}
}

#endif  // ACTFW_CONCURRENCY_PARALLEL_FOR_H
