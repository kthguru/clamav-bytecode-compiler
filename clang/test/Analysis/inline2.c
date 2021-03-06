// RUN: %clang_cc1 -analyze -inline-call -analyzer-store region -analyze-function f2 -verify %s

// Test parameter 'a' is registered to LiveVariables analysis data although it
// is not referenced in the function body. 
// Before processing 'return 1;', in RemoveDeadBindings(), we query the liveness
// of 'a', because we have a binding for it due to parameter passing.
int f1(int a) {
  return 1;
}

void f2() {
  int x;
  x = f1(1);
}
