//= PrintfFormatStrings.cpp - Analysis of printf format strings --*- C++ -*-==//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// Handling of format string in printf and friends.  The structure of format
// strings for fprintf() are described in C99 7.19.6.1.
//
//===----------------------------------------------------------------------===//

#include "clang/Analysis/Analyses/PrintfFormatString.h"

using namespace clang;
using namespace analyze_printf;

namespace {
class FormatSpecifierResult {
  FormatSpecifier FS;
  const char *Start;
  bool HasError;
public:
  FormatSpecifierResult(bool err = false)
    : Start(0), HasError(err) {}
  FormatSpecifierResult(const char *start,
                        const FormatSpecifier &fs)
    : FS(fs), Start(start), HasError(false) {}

  
  const char *getStart() const { return Start; }
  bool hasError() const { return HasError; }
  bool hasValue() const { return Start != 0; }
  const FormatSpecifier &getValue() const {
    assert(hasValue());
    return FS;
  }
  const FormatSpecifier &getValue() { return FS; }
};
} // end anonymous namespace

template <typename T>
class UpdateOnReturn {
  T &ValueToUpdate;
  const T &ValueToCopy;
public:
  UpdateOnReturn(T &valueToUpdate, const T &valueToCopy)
    : ValueToUpdate(valueToUpdate), ValueToCopy(valueToCopy) {}
  
  ~UpdateOnReturn() {
    ValueToUpdate = ValueToCopy;
  }
};  

static OptionalAmount ParseAmount(const char *&Beg, const char *E) {
  const char *I = Beg;
  UpdateOnReturn <const char*> UpdateBeg(Beg, I);
  
  bool foundDigits = false;
  unsigned accumulator = 0;

  for ( ; I != E; ++I) {
    char c = *I;
    if (c >= '0' && c <= '9') {
      foundDigits = true;
      accumulator += (accumulator * 10) + (c - '0');
      continue;
    }

    if (foundDigits)
      return OptionalAmount(accumulator);
    
    if (c == '*')
      return OptionalAmount(OptionalAmount::Arg);
    
    break;
  }
  
  return OptionalAmount();  
}

static FormatSpecifierResult ParseFormatSpecifier(FormatStringHandler &H,
                                                  const char *&Beg, const char *E) {
  
  const char *I = Beg;
  const char *Start = 0;
  UpdateOnReturn <const char*> UpdateBeg(Beg, I);

  // Look for a '%' character that indicates the start of a format specifier.
  while (I != E) {
    char c = *I;
    ++I;    
    if (c == '\0') {
      // Detect spurious null characters, which are likely errors.
      H.HandleNullChar(I);
      return true;
    }
    if (c == '%') {
      Start = I;  // Record the start of the format specifier.
      break;
    }
  }
  
  // No format specifier found?
  if (!Start)
    return false;
  
  if (I == E) {
    // No more characters left?
    H.HandleIncompleteFormatSpecifier(Start, E);
    return true;
  }
      
  FormatSpecifier FS;
  
  // Look for flags (if any).
  bool hasMore = true;
  for ( ; I != E; ++I) {
    switch (*I) {
      default: hasMore = false; break;
      case '-': FS.setIsLeftJustified(); break;
      case '+': FS.setHasPlusPrefix(); break;
      case ' ': FS.setHasSpacePrefix(); break;
      case '#': FS.setHasAlternativeForm(); break;
      case '0': FS.setHasLeadingZeros(); break;
    }
    if (!hasMore)
      break;
  }      

  if (I == E) {
    // No more characters left?
    H.HandleIncompleteFormatSpecifier(Start, E);
    return true;
  }
  
  // Look for the field width (if any).
  FS.setFieldWidth(ParseAmount(I, E));
      
  if (I == E) {
    // No more characters left?
    H.HandleIncompleteFormatSpecifier(Start, E);
    return true;
  }  
  
  // Look for the precision (if any).  
  if (*I == '.') {
    const char *startPrecision = I++;
    if (I == E) {
      H.HandleIncompletePrecision(I - 1);
      return true;
    }
    
    FS.setPrecision(ParseAmount(I, E));

    if (I == E) {
      // No more characters left?
      H.HandleIncompletePrecision(startPrecision);
      return true;
    }
  }

  // Look for the length modifier.
  LengthModifier lm = None;
  switch (*I) {
    default:
      break;
    case 'h':
      ++I;
      lm = (I != E && *I == 'h') ? ++I, AsChar : AsShort;      
      break;
    case 'l':
      ++I;
      lm = (I != E && *I == 'l') ? ++I, AsLongLong : AsLong;
      break;
    case 'j': lm = AsIntMax;     ++I; break;
    case 'z': lm = AsSizeT;      ++I; break;
    case 't': lm = AsPtrDiff;    ++I; break;
    case 'L': lm = AsLongDouble; ++I; break;
  }
  FS.setLengthModifier(lm);
  
  if (I == E) {
    // No more characters left?
    H.HandleIncompleteFormatSpecifier(Start, E);
    return true;
  }
  
  // Finally, look for the conversion specifier.
  const char *conversionPosition = I++;
  ConversionSpecifier::Kind k;
  switch (*conversionPosition) {
    default:
      H.HandleInvalidConversionSpecifier(conversionPosition);
      return true;      
    // C99: 7.19.6.1 (section 8).
    case 'd': k = ConversionSpecifier::dArg; break;
    case 'i': k = ConversionSpecifier::iArg; break;
    case 'o': k = ConversionSpecifier::oArg; break;
    case 'u': k = ConversionSpecifier::uArg; break;
    case 'x': k = ConversionSpecifier::xArg; break;
    case 'X': k = ConversionSpecifier::XArg; break;
    case 'f': k = ConversionSpecifier::fArg; break;
    case 'F': k = ConversionSpecifier::FArg; break;
    case 'e': k = ConversionSpecifier::eArg; break;
    case 'E': k = ConversionSpecifier::EArg; break;
    case 'g': k = ConversionSpecifier::gArg; break;
    case 'G': k = ConversionSpecifier::GArg; break;
    case 'a': k = ConversionSpecifier::aArg; break;
    case 'A': k = ConversionSpecifier::AArg; break;
    case 'c': k = ConversionSpecifier::IntAsCharArg; break;
    case 's': k = ConversionSpecifier::CStrArg;      break;
    case 'p': k = ConversionSpecifier::VoidPtrArg;   break;
    case 'n': k = ConversionSpecifier::OutIntPtrArg; break;
    case '%': k = ConversionSpecifier::PercentArg;   break;      
    // Objective-C.
    case '@': k = ConversionSpecifier::ObjCObjArg; break;      
  }
  FS.setConversionSpecifier(ConversionSpecifier(conversionPosition, k));
  return FormatSpecifierResult(Start, FS);
}

namespace clang { namespace analyze_printf {
bool ParseFormatString(FormatStringHandler &H,
                       const char *I, const char *E) {
  // Keep looking for a format specifier until we have exhausted the string.
  while (I != E) {
    const FormatSpecifierResult &FSR = ParseFormatSpecifier(H, I, E);
    // Did an error of any kind occur when parsing the specifier?  If so,
    // don't do any more processing.
    if (FSR.hasError())
      return true;;
    // Done processing the string?
    if (!FSR.hasValue())
      break;    
    // We have a format specifier.  Pass it to the callback.
    if (!H.HandleFormatSpecifier(FSR.getValue(), FSR.getStart(),
                                 I - FSR.getStart()))
      return false;
  }  
  assert(I == E && "Format string not exhausted");      
  return false;
}
}}

FormatStringHandler::~FormatStringHandler() {}
