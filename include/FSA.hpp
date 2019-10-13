// FSA.hpp
// Einfache Klasse zur Repr�sentation endlicher (nicht-determ.) Automaten
// V4: std::vector anstatt statischer C-Vektoren, fsa_closure(),
//     bin�res Lesen und Schreiben
// V5: Zustands - und �bergangsiteratoren
// V6: �berg�nge sind nun alphabet. sortiert
// V7: Hash-Funktion hinzugef�gt
// V8: Templatisierung
// TH, 8.7.19
// modified by SP. 2.10.19
// made state vector public.. sorry

#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cassert>
#include <functional> // f�r std::hash
// #include <boost/functional/hash.hpp>  // f�r hash_combine, hash_range

/// ID f�r bin�re Dateien
const std::string FSA_ID = "FSA Copyright UP 2019";


/// FSA realisiert einen nicht-determ. endl. Akzeptor
template <typename State=unsigned int> 
class FSA
{
public: // �ffentliche Typdefinition 
  //typedef unsigned int                          State;
  typedef char                                  Symbol;
  typedef enum { opConcat, opUnion, opClosure } FSAOperation;

private: // Private
  /// Zustandsbeschreibung
  struct InternalState
  {
    InternalState() : is_final(false), first_tr_idx(-1), last_tr_idx(-1) {}
    size_t hash() const
    {
      std::hash<int> hash_int;
      return std::hash<bool>()(is_final) ^ hash_int(first_tr_idx) ^ hash_int(last_tr_idx); 
    }
    
    bool  is_final;     ///< Ist der Zustand ein Endzustand?
    int   first_tr_idx; ///< An dieser Stelle in transitions befindet sich der erste �bergang
    int   last_tr_idx;  ///< An dieser Stelle in transitions befindet sich der letzte �bergang
  }; // State

  /// Repr�sentiert einen �bergang in einer Adjazenzliste (in der logischen Sicht von au�en)
  struct Transition
  {
    Transition() : letter(0), target_state(-1) { }
    Transition(Symbol s, State q) : letter(s), target_state(q) {}
    Symbol    letter;         ///< Symbol am �bergang
    State     target_state;   ///< Erreichter Zustand
    size_t hash() const
    {
      return std::hash<Symbol>()(letter) ^ std::hash<State>()(target_state); 
    }
    // NB. Es w�re nicht schlecht, hier auch einen Ausgabeoperator zu definieren. Leider ist 
    // der �bergang aufgrund unserer Repr�sentation unvollst�ndig, da wir den Ausgangszustand nicht
    // repr�sentieren
  }; // Transition

  /// Repr�sentiert einen �bergang in einer Adjazenzliste (in der Innensicht/Verwaltungsansicht)
  struct InternalTransition
  {
    /// Konstruktor mit Defaultargumenten
    InternalTransition(const Transition& t = Transition(), int n = -1) : transition(t), next(n) { }
    size_t hash() const { return transition.hash() ^ std::hash<int>()(next); }
    Transition transition;  ///< der externe �bergang
    int        next;        ///< der Index des n�chsten �bergangs in unserer internen Struktur
  }; // InternalTransition
     
  /// Realisiert einen �bergangsiterator
  class ConstTransitionIterator
  {
  public:
    /// Konstruktor: �ber aktuellen �bergangsindex und Automaten, �ber den iteriert wird
    ConstTransitionIterator(int ti, const FSA& fsa) : tr_idx(ti), my_fsa(fsa) {}
    
    /// Test auf Gleichheit zweier Iteratoren
    bool operator==(const ConstTransitionIterator& t) const
    {
      /// Indices sind gleich und auch der Automat ist derselbe (gleiche Adressen)
      return tr_idx == t.tr_idx && &this->my_fsa == &t.my_fsa;
    }
    
    /// Test auf Ungleichheit zweier Iteratoren
    bool operator!=(const ConstTransitionIterator& t) const
    {
      /// Wir testen auf Wertgleichheit, nicht Identit�t
      return !(*this == t);
    }
    
    /// Pr�fix-Inkrement
    void operator++() 
    {
      if (tr_idx == -1) return;
      tr_idx = my_fsa.next_transition_index(tr_idx);
    }
    
    /// Dereferenzierung des Iterators
    const Transition& operator*() const
    {
      assert(tr_idx != -1);
      return my_fsa.transitions[tr_idx].transition;
    }

    /// Dereferenzierung des Iterators
    const Transition* operator->() const
    {
      assert(tr_idx != -1);
      return &my_fsa.transitions[tr_idx].transition;
    }

  private:
    int         tr_idx;   ///< Der aktuelle �bergangsindex, oder -1
    const FSA&  my_fsa;   ///< Die Instanz von FSA, �ber die iteriert wird
  }; // ConstTransitionIterator

public: // Konstanten
  /// Symbol f�r Epsilon�berg�nge
  const static Symbol EPSILON = 0;

public: // Konstruktoren, Schnittstelle
  /// Default-Konstruktor: erzeugt einen Akzeptor mit Startzustand
  FSA()
  {
    start = new_state();
  }

  /// Konstruktion eines FSA entweder 
  /// a) aus einem Textstream von reg. Ausdr�cken oder
  /// b) aus einem Bin�rstream
  /// 'in' muss ein bin�r ge�ffneter Stream sein, wenn binary == true, sonst ein Textstream
  FSA(std::ifstream& in, bool binary) : FSA()
  {
    if (binary) {
      if (!deserialize(in)) {
        // Wenn Einlesen nicht geklappt hat, alles zur�cksetzen
        // NB. Hier sollte man sp�ter noch mehr machen.
        reset();
      }
    }
    else {
      // Regex-Liste einlesen (im Moment nur eine Wortliste)
      add_regexes_from(in);
    }
  }

  /// Konstruktion eines FSA aus einem String s
  /// NB. Im Moment ist das nur ein String, k�nnte sp�ter aber auch ein
  /// Regex-Compiler sein (Thompson-Konstruktion u.dgl.)
  FSA(std::string s) : FSA()
  {
    add(s);
  }

  /// Kopierkonstruktor: nicht l�nger n�tig, da Standardsemantik 
  /// ("rekursiver" Aufruf der Kopierkonstruktoren der Instanzvariablen) ausreichend
  // FSA(const FSA& rhs) { }

  /// Zuweisung: nicht l�nger n�tig (s.o.)
  //const FSA& operator=(const FSA& rhs)
  //{
  //  if (this != &rhs) { ... }
  //  return *this;
  //}

  /// Gibt den Startzustand zur�ck
  State start_state() const
  {
    return start;
  }

  /// Anzahl der Zust�nde
  unsigned no_states() const
  {
    return states.size();
  }
  
  /// Anzahl der Zust�nde
  unsigned no_transitions() const
  {
    return transitions.size();
  }
  
  /// Anzahl der Endzust�nde
  unsigned no_final_states() const
  {
    unsigned n = 0;
    for (State q = 0; q < states.size(); ++q) {
      if (is_final(q)) ++n;
    }
    return n;
  }

  State get_final_state() const
  {
    unsigned n = 0;
    for (State q = 0; q < states.size(); ++q) {
      if (is_final(q)) return q;
    }
  }



public: // Algebra  
  /// Verkettet *this konstruktiv mit rhs (mittels Epsilon-�berg�ngen)
  /// und gibt das Ergebnis zur�ck
  FSA fsa_concat(const FSA& rhs) const
  {
    // Aufruf des privaten Hilfskonstruktors
    return FSA(*this,rhs,opConcat);
  }
    
  /// Vereinigt *this konstruktiv mit rhs (mittels Epsilon-�berg�ngen)
  /// und gibt das Ergebnis zur�ck
  FSA fsa_union(const FSA& rhs) const
  {
    // Aufruf des privaten Hilfskonstruktors
    return FSA(*this,rhs,opUnion);
  }

  /// Konstruktive Sternh�lle
  FSA fsa_closure() const
  {
    FSA fsa(*this); // Kopie mit Kopierkonstruktor
    fsa.do_star_closure(); // Sternh�lle (destruktiv)
    return fsa;
  }

public: // Weitere Funktionen
  /// Schreiben eines Speicherauszugs in eine Bin�rdatei.
  /// bin_out muss ein bin�r ge�ffneter Stream sein
  /// Hinweis: serialize() schlie�t die Datei nicht
  /// NB. Datenrepr�sentation ist noch nicht sehr portabel
  void serialize(std::ofstream& bin_out) const
  {
    // ID schreiben
    bin_out.write(FSA_ID.c_str(), FSA_ID.size());
    
    // Metadaten: Zustands- und �bergangsanzahl
    auto n_states = states.size();
    bin_out.write((const char*) &n_states, sizeof(n_states));
    auto n_trans = transitions.size();
    bin_out.write((const char*) &n_trans, sizeof(n_trans));
    
    // Daten schreiben (wichtig: Test, ob auch Daten da sind)
    // Startzustand
    bin_out.write((const char*) &start, sizeof(start));
    
    if (n_states > 0) {
      bin_out.write((const char*) &states[0], n_states * sizeof(InternalState));
    }
    if (n_trans > 0) {
      bin_out.write((const char*) &transitions[0], n_trans * sizeof(InternalTransition));
    }
  }
    
  /// Alles zur�cksetzen
  void reset()
  {
    transitions.clear();
    states.clear();
    start = new_state();
  }

  /// Gibt die ID des ersten Zustands zur�ck
  State states_begin() const
  {
    return 0;
  }

  /// Gibt die ID des Zustands nach dem letzten zur�ck
  State states_end() const
  {
    return states.size();
  }

  /// Gibt einen Iterator auf den ersten von q ausgehenden �bergang zur�ck
  ConstTransitionIterator transitions_leaving(State q) const
  {
    return ConstTransitionIterator(first_transition_index(q), *this);
  }

  /// Gibt einen Iterator auf den �bergang nach dem letzten zur�ck
  ConstTransitionIterator end_transitions() const
  {
    return ConstTransitionIterator(-1,*this);
  }
  
  /// Ausgabeoperator
  friend std::ostream& operator<<(std::ostream& o, const FSA& fsa)
  {
    // Hier nur eine Kurzbeschreibung des Automaten; man k�nnte auch JSON usw.
    // ausgeben. Eine Unterscheidung zwischen __str__ und __repr__ wie in Python
    // gibt es nicht
    o << "FSA(" << "states=" << fsa.no_states() << 
         ", transitions=" << fsa.no_transitions() << ")";
    return o;
  }

  /// Gibt eine Graphviz-dot-Repr�sentation aus
  void as_dot(std::ostream& dot) const
  {
    dot << "digraph trie {\n";
    dot << "rankdir = LR\n";
    dot << "node [shape=circle]\n";
    // NB. dot-Grafiken sehen besser aus, wenn man den Startzustand zuerst ausgibt
    dot << "start [style=invis]\n";
    dot << start_state() << "\n";
    dot << "start -> " << start_state() << "\n";

    for (State q = 0; q < states.size(); ++q) {
      if (is_final(q)) {
        dot << q << " [shape=doublecircle]\n";
      }
      for (auto t = transitions_leaving(q); t != end_transitions(); ++t) {
        std::string sym = (t->letter != EPSILON) ? std::string(1, t->letter) : "EPSI"; // Epsilon
        dot << q << " -> " << t->target_state << " [label=\"" << sym << "\"]\n";
      }
    } // for q
    dot << "}\n";
  }
  
  /// Hash-Funktion f�r Instanzen fon FSA
  size_t hash() const
  {
    // Hash-Register mit Startzustand initialisieren
    size_t h = std::hash<State>()(start);
    // Alle internen Zust�nde ins Hash-Register integrieren
    for (auto q = states.begin(); q != states.end(); ++q) {
      h ^= q->hash() + 0x9e3779b9 + (h<<28) + (h>>4);
    }
    // Alle internen �berg�nge ins Hash-Register integrieren
    for (auto t = transitions.begin(); t != transitions.end(); ++t) {
      h ^= t->hash() + 0x9e3779b9 + (h<<28) + (h>>4);
    }
    return h;
  }

private: // Funktionen
  /// Einlesen aus Bin�rdatei
  bool deserialize(std::ifstream& bin_in) 
  {
    // Test, ob die Datei auch eine FSA-Datei ist
    std::string tmp(FSA_ID.size(), 0); // Platz schaffen f�r |FSA_ID| Bytes
    bin_in.read(&tmp[0],FSA_ID.size()); // Die ersten |FSA_ID| Bytes einlesen
    if (tmp != FSA_ID) {
      std::cerr << "FATAL: FSA file is corrupt\n";
      return false;
    }
    
    // Metadaten
    auto n_states = states.size();
    auto n_trans = transitions.size();
    
    bin_in.read((char*) &n_states, sizeof(n_states));
    bin_in.read((char*) &n_trans, sizeof(n_trans));
    bin_in.read((char*) &start, sizeof(start));
    
    // Vektoren
    if (n_states > 0) {
      states.resize(n_states);
      bin_in.read((char*) &states[0], n_states * sizeof(InternalState));
    }
    
    if (n_trans > 0) {
      transitions.resize(n_trans);
      bin_in.read((char*) &transitions[0], n_trans * sizeof(InternalTransition));
    }
    
    // Wir m�ssen jetzt am Dateiende sein
    return true; // bin_in.eof();
  }

  /// Verkettungskonstruktor (kopiert zun�chst fsa1)
  FSA(const FSA& fsa1, const FSA& fsa2, FSAOperation op) : FSA(fsa1)
  {
    if (op == opConcat) {
      // Verkette fsa2 Zust�nde und �berg�nge mit Offset
      do_concat(fsa2);
    }
    else if (op == opUnion) {
      // Vereinige fsa2 Zust�nde und �berg�nge mit Offset
      do_union(fsa2);
    }
  }

  /// Konstruiert die Vereinigung aller reg. Ausdr�cke im Textstream 'in'
  /// NB. Wortlisten enthalten Eintr�ge, die auf impliziter Verkettung beruhen;
  /// unter den Eintr�gen besteht ein implizites Disjunktionsverh�ltnis
  void add_regexes_from(std::istream& in)
  {
    std::string line;
    while (std::getline(in,line)) {
      add(line);
    }
  }

  /// String hinzuf�gen
  //template<typename Iter>
  //void add(Iter start, Iter stop) 
  void add(const std::string& word) 
  { 
    // Wir beginnen beim Startzustand
    State current = start_state();
    for (auto i = 0; i < word.size(); ++i) {
      // Versuche, n�chsten Zustand zu bestimmen
      auto next = next_state(current,word[i]);
      if (next == State(-1)) {
        // Kein n�chster Zustand => �bergang samt Zielzustand erzeugen
        next = add_transition(current,word[i]);
      }
      current = next;
    }
    // Erreichten Zustand zum Endzustand machen
    set_finality(current,true); 
  }
  
  /// Destruktive Verkettung 
  void do_concat(const FSA& fsa2)
  {
    if (no_final_states() == 0 || fsa2.no_final_states() == 0) return;
    auto new_start = new_state();
    //auto new_start = fsa2.get_final_state();
    auto new_final = new_state();
    auto state_offset = no_states();

    add_transition(new_start, EPSILON, start_state());

    // Verbinde Endzust�nde von fsa1 mit Startzustand von fsa2 
    for (auto q = 0; q < states.size(); ++q) {
      if (states[q].is_final) {
        add_transition(q,EPSILON,fsa2.start_state()+state_offset);
        set_finality(q,false);
      }
    } // for

    // Kopiere fsa2
    copy(fsa2, state_offset);

    for (auto q = 0; q < fsa2.states.size(); ++q) {
      if (fsa2.states[q].is_final) {
        add_transition(q + state_offset, EPSILON, new_final);
        set_finality(q + state_offset, false);
      }
    } // for

    set_finality(new_final, true);
    start = new_start;
  }

  /// Destruktive Vereinigung mit fsa2
  void do_union(const FSA& fsa2)
  {
    auto state_offset = no_states();
    copy(fsa2, state_offset);

    // Neuen Startzustand erzeugen 
    // und mit Epsilon mit den beiden alten Startzust�nden verbinden
    auto new_start = new_state();
    add_transition(new_start, EPSILON, start_state());
    add_transition(new_start, EPSILON, fsa2.start_state()+state_offset);    
    // Endzust�nde mit neuem Endzustand verbinden
    auto new_final = new_state();
    for (auto q = 0; q < states.size(); ++q) {
      if (states[q].is_final) {
        add_transition(q,EPSILON,new_final);
        set_finality(q,false);
      }
    } // for
    set_finality(new_final,true);
    start = new_start;
  }

  /// Destruktive Sternh�lle
  /// hack this shit
  void do_star_closure()
  {
    auto new_start = new_state();
    auto new_final = new_state();
    add_transition(new_start, EPSILON, start_state());
    add_transition(new_start, EPSILON, new_final);
    for (auto q = 0; q < states.size(); ++q) {
      if (states[q].is_final && State(q) != start_state()) {
        add_transition(q,EPSILON, start_state());
        add_transition(q, EPSILON, new_final);
        set_finality(q, false);
      }
    } // for 
    set_finality(new_final, true);
    start = new_start;
  }

  /// Kopiert alle Zust�nde und �berg�nge
  /// NB. Momentan nehmen wir an, dass die Zustandsvektoren keine "L�cher", d.h. unbenutzte Zust�nde haben
  void copy(const FSA& fsa2, unsigned offset)
  {
    // Vektoren anpassen (so dass fsa2 auch "reinpasst")
    states.resize(states.size()+fsa2.states.size());
    transitions.resize(transitions.size()+fsa2.transitions.size());
    
    // fsa2 transponiert kopieren (d.h. seine Zust�nde um 'offset' erh�hen)
    for (State q = 0; q < fsa2.no_states(); ++q) {
      if (fsa2.is_final(q)) {
        set_finality(q+offset,true);
      }
      for (auto t_idx = fsa2.first_transition_index(q); t_idx != -1; t_idx = fsa2.next_transition_index(t_idx)) {
        add_transition(q+offset,fsa2.transitions[t_idx].transition.letter,
                       fsa2.transitions[t_idx].transition.target_state + offset);
      }
    } // for q
  }

  /// Gibt true zur�ck gdw. q ein Endzustand ist
  bool is_final(State q) const
  {
    assert(q >= 0 && q < states.size());
    return states[q].is_final;
  }
  
  /// Gibt true zur�ck gdw. q ein Endzustand ist
  bool set_finality(State q, bool v)
  {
    assert(q >= 0 && q < states.size());
    if (states[q].is_final == v) return false;
    states[q].is_final = v;
    return true;
  }
  
  /// Gibt den Nachfolgezustand f�r 'state' mit Symbol 'sym' zur�ck;
  /// die R�ckgabe ist -1, wenn dieser nicht exisiert
  State next_state(State state, Symbol sym) const
  {
    assert(state >= 0 && state < states.size());
    // Adjazenzliste von state durchlaufen
    for (int idx = states[state].first_tr_idx; idx != -1; idx = transitions[idx].next) {
      // Pr�fen, ob �bergangssymbol mit dem gesuchten �bereinstimmt
      if (transitions[idx].transition.letter == sym)
        // Ja, Zielzustand zur�ckgeben
        return transitions[idx].transition.target_state;
    }
    // Nicht gefunden
    return State(-1);
  }

  /// Gibt einen neuen Zustand zur�ck (oder -1)
  State new_state()
  {
    // Neuen defaultkonstruierten Zustand hinten anh�ngen
    // NB. Wir verlassen uns auf die Allokationsstrategie von std::vector bzgl. Kapazit�tserweiterung
    states.push_back(InternalState());
    // Index des neuen Zustands zur�ckgeben
    return states.size()-1;
  }

  /// Erzeuge einen neuen �bergang mit sym zwischen 'from' und 'to'
  State add_transition(State from, Symbol sym, State to) 
  {
    // Gr��eren der beiden Zust�nde bestimmen und states-Vektor ggf. wachsen lassen
    State last = (from > to) ? from : to;
    if (last >= states.size()) {
      // Anmerkung: Hier k�nnte man ggf. auch wieder eine etwas gr��ere Kapazit�t anlegen
      states.resize(last+1);
    }
    // NEU: wir f�gen in alphabet. Reihenfolge ein
    if (states[from].first_tr_idx == -1) {
      // Erster �bergang �berhaupt
      transitions.push_back(InternalTransition(Transition(sym,to),-1));
      states[from].first_tr_idx = states[from].last_tr_idx = transitions.size()-1;
    }
    else {
      // Es gibt bereits �berg�nge => Einf�gestelle suchen: 
      // idx zeigt auf den �bergang, wo sich der neue �bergang anschlie�t
      auto idx = transition_insertion_position(from,sym);
      if (idx == -1) {
        // Einf�gen am Anfang
        transitions.push_back(InternalTransition(Transition(sym,to),states[from].first_tr_idx));
        states[from].first_tr_idx = transitions.size()-1;
      }
      else {
        // Einf�gen in der Mitte oder nach dem letzten
        transitions.push_back(InternalTransition(Transition(sym,to),transitions[idx].next));
        transitions[idx].next = transitions.size()-1;
        if (idx == states[from].last_tr_idx) {
          states[from].last_tr_idx = transitions.size()-1; // Neuer letzter �bergang
        }
      }
    }
    // Zielzustand zur�ckgeben
    return to;
  }

  /// Erzeuge einen neuen �bergang mit sym von from aus und gib einen neuen Zustand zur�ck
  State add_transition(State from, Symbol sym) 
  {
    return add_transition(from,sym,new_state());
  }

  /// Lineare Suche f�r die alphabetische korrekte Einf�geposition
  /// R�ckgabewert ist der �bergangsindex, nach dessen �bergang sym einsortiert wird
  int transition_insertion_position(State from, Symbol sym) const
  {
    // Idee: prev_idx l�uft idx um eine Position hinterher
    int prev_idx = -1;
    for (int idx = states[from].first_tr_idx; idx != -1; idx = transitions[idx].next) {
      // Pr�fen, ob �bergangssymbol mit dem gesuchten �bereinstimmt
      if (transitions[idx].transition.letter >= sym) {
        return prev_idx;
      }
      prev_idx = idx;
    }
    return prev_idx;
  }

  int first_transition_index(State q) const
  {
    assert(q >= 0 && q < states.size());
    return states[q].first_tr_idx;
  }

  int next_transition_index(int idx) const
  {
    assert(idx >= 0 && idx < transitions.size());
    return transitions[idx].next;    
  }

public: // Instanzvariablen
  std::vector<InternalState>        states;       ///< Zustandsvektor
  std::vector<InternalTransition>   transitions;  ///< �bergangsvektor
  State                             start;        ///< Startzustand
}; // FSA
