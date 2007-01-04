
#ifndef DN_MAIN_H
#define DN_MAIN_H

extern class Dn_Memory *mem;
extern class Dn_CPU *cpu;
extern class Dn_Symbols *symbols;

extern void save_gpio(const char *filename);

extern void open_snapshot();
extern void save_snapshot();

extern void show_preferences(const char *reason=0L);
extern void load_preferences();
extern void save_preferences();

extern void desym(const char *pathname);

extern void read_symbols();
extern void add_edit_comment();

#endif

