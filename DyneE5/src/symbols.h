
#ifndef DN_SYMBOLS_H
#define DN_SYMBOLS_H

#ifdef WIN32
# pragma warning ( disable : 4786 )
#endif


#include <map>


class Dn_Symbols {

public:
  typedef std::map < unsigned int, const char* > Dn_S_Symbol;
  Dn_S_Symbol list;
  typedef std::map < unsigned int, const char* > Dn_S_Comment;
  Dn_S_Comment comments;
  typedef std::map < unsigned int, unsigned int > Dn_S_Flags;
  Dn_S_Flags flags;
  typedef void (*details_fn)(unsigned int addr);
  typedef std::map < unsigned int, details_fn > Dn_S_Details;
  Dn_S_Details details;

  typedef std::map < unsigned int, unsigned int > Dn_S_Tick;
  Dn_S_Tick ticks;

  void set_flags(unsigned int addr, unsigned int ff);
  unsigned int get_flags(unsigned int addr);

public:
  Dn_Symbols();

  void clear();
  const char *get(unsigned int addr);
  void add(unsigned int add, const char *text);
  void load(const char *filename);
  void reload(const char *filename) { clear(); load(filename); }

  unsigned char is_breakpoint(unsigned int addr);
  void set_breakpoint(unsigned int addr);
  void clear_breakpoint(unsigned int addr);

  unsigned char is_tmp_breakpoint(unsigned int addr);
  void set_tmp_breakpoint(unsigned int addr);
  void clear_tmp_breakpoint(unsigned int addr);

  unsigned char is_watchpoint(unsigned int addr);
  void set_watchpoint(unsigned int addr);
  void clear_watchpoint(unsigned int addr);

  void add_detail(unsigned int addr, details_fn f);
  void print_details(unsigned int addr);

  void tick(unsigned int addr);
  void save_ticks(const char *filename);

  const char *get_comment(unsigned int addr);
};


#endif

