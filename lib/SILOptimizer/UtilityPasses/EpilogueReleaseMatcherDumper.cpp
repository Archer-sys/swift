//===--- EpilogueReleaseMatcherDumper.cpp - Find Epilogue Releases  -------===//
//
// This source file is part of the Swift.org open source project
//
// Copyright (c) 2014 - 2016 Apple Inc. and the Swift project authors
// Licensed under Apache License v2.0 with Runtime Library Exception
//
// See http://swift.org/LICENSE.txt for license information
// See http://swift.org/CONTRIBUTORS.txt for the list of Swift project authors
//
//===----------------------------------------------------------------------===//
///
/// \file
/// This pass finds the epilogue releases matched to each argument of the
/// function.
///
//===----------------------------------------------------------------------===//

#define DEBUG_TYPE "sil-epilogue-release-dumper"
#include "swift/SILOptimizer/PassManager/Passes.h"
#include "swift/SIL/SILArgument.h"
#include "swift/SIL/SILFunction.h"
#include "swift/SIL/SILValue.h"
#include "swift/SILOptimizer/Analysis/RCIdentityAnalysis.h"
#include "swift/SILOptimizer/Analysis/ARCAnalysis.h"
#include "swift/SILOptimizer/Analysis/Analysis.h"
#include "swift/SILOptimizer/PassManager/Transforms.h"

using namespace swift;

//===----------------------------------------------------------------------===//
//                              Top Level Driver
//===----------------------------------------------------------------------===//

namespace {

/// Find and dump the epilogue release instructions for the arguments.
class SILEpilogueReleaseMatcherDumper : public SILModuleTransform {

  void run() override {
    auto *RCIA = getAnalysis<RCIdentityAnalysis>();
    for (auto &Fn: *getModule()) {
      // Function is not definition.
      if (!Fn.isDefinition())
        continue;

      llvm::outs() << "START: sil @" << Fn.getName() << "\n";
      ConsumedArgToEpilogueReleaseMatcher ArgToRetRelMap(RCIA->get(&Fn), &Fn); 

      // Iterate over arguments and dump their epilogue releases.
      for (auto Arg : Fn.getArguments()) {
        llvm::outs() << *Arg;
        // Can not find an epilogue release instruction for the argument.
        for (auto &RI : ArgToRetRelMap.getReleasesForArgument(Arg))
          llvm::outs() << *RI;
      }

      llvm::outs() << "END: sil @" << Fn.getName() << "\n";
    }
  }

  StringRef getName() override { return "Epilogue Release Matcher Dumper"; }
};
        
} // end anonymous namespace

SILTransform *swift::createEpilogueReleaseMatcherDumper() {
  return new SILEpilogueReleaseMatcherDumper();
}