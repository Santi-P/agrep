// FSAWrapper.hpp
// TH, UP 2019
// Funktionales Wrapping der FSA<STATE>-Klasse
// Compiler: gcc

#pragma once

#include "FSA.hpp"

template<typename STATE>
FSA<STATE> fsa_union(FSA<STATE> a, const FSA<STATE>& b)
{
  return a.fsa_union(b);
}

template<typename STATE>
FSA<STATE> fsa_concat(const FSA<STATE>& a, const FSA<STATE>& b)
{
  return a.fsa_concat(b);
}

template<typename STATE>
FSA<STATE> fsa_closure(FSA<STATE> a)
{
  return a.fsa_closure();
}

/// Globaler Verkettungsoperator für FSAs
template<typename STATE>
FSA<STATE> operator*(const FSA<STATE>& a, const FSA<STATE>& b)
{
  return fsa_concat(a,b);
}

/// Globaler Vereinigungsoperator für FSAs
template<typename STATE>
FSA<STATE> operator+(const FSA<STATE>& a, const FSA<STATE>& b)
{
  return fsa_union(a,b);
}  

/// Asym. Überladungen
template<typename STATE>
FSA<STATE> operator+(const std::string& s, const FSA<STATE>& b)
{
  return fsa_union(FSA<STATE>(s),b);
}  

template<typename STATE>
FSA<STATE> operator+(const FSA<STATE>& a, const std::string& s)
{
  return fsa_union(a,FSA<STATE>(s));
}  

template<typename STATE>
FSA<STATE> operator*(const std::string& s, const FSA<STATE>& b)
{
  return fsa_concat(FSA<STATE>(s),b);
}  

template<typename STATE>
FSA<STATE> operator*(const FSA<STATE>& a, const std::string& s)
{
  return fsa_concat(a,FSA<STATE>(s));
}  
