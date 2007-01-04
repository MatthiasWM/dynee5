
#include "main.h"
#include "mainc.h"
#include "dyne.h"
#include "memory.h"
#include "symbols.h"

#include <FL/fl_ask.H>
#include <FL/fl_file_chooser.H>
#include <FL/Fl_Window.H>

#include "cpu.h"

# include <stdarg.h>

Dn_Memory *mem = 0L;
Dn_CPU *cpu = 0L;
Dn_Symbols *symbols = 0L;

void err_msg(const char *a, ...) {
  va_list args;
  va_start(args, a);
  vprintf(a, args);
  va_end(args);
}

void d_msg(const char *a, ...) {
/*
  va_list args;
  va_start(args, a);
  vprintf(a, args);
  va_end(args);
*/
}


// interface between "C" and "C++"
void dn_set32(unsigned int a, unsigned int d) {
  mem->set32(a, d);
}

// interface between "C" and "C++"
void dn_set16(unsigned int a, unsigned int d) {
  mem->set16(a, d);
}

// interface between "C" and "C++"
void dn_set8(unsigned int a, unsigned int d) {
  mem->set8(a, d);
}


// interface between "C" and "C++"
unsigned int dn_get32(unsigned int a) {
  return mem->get32(a);
}

// interface between "C" and "C++"
unsigned int dn_get16(unsigned int a) {
  return mem->get16(a);
}

// interface between "C" and "C++"
unsigned int dn_get8(unsigned int a) {
  return mem->get8(a);
}

// interface between "C" and "C++"
void dn_begin_instr_fetch() {
  mem->begin_instr_fetch();
}

// interface between "C" and "C++"
void dn_end_instr_fetch() {
  mem->end_instr_fetch();
}

const char *dn_get_symbol(unsigned int a) {
  return symbols->get(a);
}

Dn_GPIO_Chunk *cGPIO;
Dn_Flash_Chunk *cFlash;
Dn_Memory_Chunk *cRAM;

void save_gpio(const char *fn) {
  cGPIO->save(fn);
}


/**
 * Load the preferences, show th dialog, and optionally
 * show another message dialog, explaining why the 
 * preferences must be modified.
 */
void show_preferences(const char *reason)
{
  load_preferences();
  wPreferences->show();
  if (reason) {
    fl_message(reason);
  }
  while (wPreferences->shown()) Fl::wait();
}


/**
 * Load all the user settings. This includes all filenames and filepaths
 * to our resources, log files, debugging behavior, etc. .
 */
void load_preferences()
{
  char buf[2048];
  int vi;
  if (!wPreferences) {
    create_preferences_dialog();
    Fl_Preferences prefs(Fl_Preferences::USER, "matthiasm.com", "DyneE5");
    Fl_Preferences memory(prefs, "memory");
    memory.get("ROM", buf, "../mem/717006", 2047);
    wFilenameROM->value(buf);
    memory.get("RAM", buf, "../mem/system_ram", 2047);
    wFilenameRAM->value(buf);
    memory.get("SaveRAM", vi, 1);
    wSaveRAM->value(vi);
    memory.get("IntFlash", buf, "../mem/internal_flash", 2047);
    wFilenameFlash->value(buf);
    memory.get("SaveFlash", vi, 1);
    wSaveFlash->value(vi);
    Fl_Preferences symbols(prefs, "symbols");
    symbols.get("symbols", buf, "../docs/MP2100Image.sym", 2047);
    wFilenameSymbols->value(buf);
    symbols.get("comments", buf, "../src/comments", 2047);
    wFilenameComments->value(buf);
    symbols.get("breakpoints", buf, "../src/breakpoints", 2047);
    wFilenameBreakpoints->value(buf);
  }
}


/**
 * Save all the user settings. This includes all filenames and filepaths
 * to our resources, log files, debugging behavior, etc. .
 */
void save_preferences()
{
  if (!wPreferences) 
    return;
  Fl_Preferences prefs(Fl_Preferences::USER, "matthiasm.com", "DyneE5");
  Fl_Preferences memory(prefs, "memory");
  memory.set("ROM", wFilenameROM->value());
  memory.set("RAM", wFilenameRAM->value());
  memory.set("SaveRAM", wSaveRAM->value());
  memory.set("IntFlash", wFilenameFlash->value());
  memory.set("SaveFlash", wSaveFlash->value());
  Fl_Preferences symbols(prefs, "symbols");
  symbols.set("symbols", wFilenameSymbols->value());
  symbols.set("comments", wFilenameComments->value());
  symbols.set("breakpoints", wFilenameBreakpoints->value());
}


/**
 * Reserve the first 8MB chunk of memory for the ROM.
 * Then load the ROM file given by the preferences. If that fails,
 * tell the user and open the prefs dialog.
 */
void create_ROM_chunk() {
  Dn_Memory_Chunk *cROM = new Dn_ROM_Chunk(0, 8*1024*1024);
  mem->add_chunk(cROM);
  char ret = cROM->load(wFilenameROM->value());
  if (ret==0) {
    show_preferences("ROM file not found or invalid. Please choose a valid binary image or ARM .aif file.");
    ret = cROM->load(wFilenameROM->value());
    if (ret==0)
      fl_alert("ROM file not found or invalid. DyneE5 will not run correctly.");
  }
}


/**
 * Reserve some memory for the screen display.
 * This chunk is an overlay to the system RAM chunk.
 * FIXME: This memory chunk is currently hardcoded in its loaction. It 
 * should however move to the DMA position written by the OS. This 
 * particular location correctly display the "Erase Flash" dialog.
 */
void create_screen_chunk() {
  Dn_Screen_Chunk *cScreen = new Dn_Screen_Chunk(0x043c0000, 320*480/2); // overlaps the RAM
  mem->add_chunk(cScreen);
  wScreen->set_content(cScreen->image());
}


/**
 * This chunk contains 4MB of volatile system RAM.
 * FIXME: we save this memory to a file, if provided, so we
 * can do faster soft resets.
 */
void create_RAM_chunk() {
  cRAM = new Dn_Memory_Chunk(0x04000000, 4*1024*1024);
  cRAM->load(wFilenameRAM->value(), 1024*1024*4);
  mem->add_chunk(cRAM);
}


/**
 * This chunk of RAM contains 4MB of non-volatile internal Flash memory.
 * If no Flash RAM file exists, all settings and installed applications
 * will be deleted between restarts of the emulator.
 */
void create_flash_chunk() {
  cFlash = new Dn_Flash_Chunk(0x02000000, 4*1024*1024);
  cFlash->load(wFilenameFlash->value());
  mem->add_chunk(cFlash);
}


/**
 * This is the chunk of memory that accesses peripherals via custom
 * chips. We need these addresses for basic power management, interrupts,
 * but also PCMCIA support und DMA.
 */
void create_GPIO_chunk() {
  cGPIO = new Dn_GPIO_Chunk(0x0f000000, 0x00400000);
  mem->add_chunk(cGPIO);
}


/**
 * Gather all the symbolic information that we can find.
 */
void read_symbols() {
  symbols = new Dn_Symbols();
  symbols->load(wFilenameSymbols->value());
  symbols->load(wFilenameComments->value());
  symbols->load(wFilenameBreakpoints->value());
}


/**
 * Add or edit a comment at the current PC.
 * FIXME: we currently support only adding a comment.
 */
void add_edit_comment() {
  const char *fn = wFilenameComments->value();
  if (!fn || !*fn) {
    fl_message("You can't add comments. Please define a comments file in the preferences first.");
    return;
  }
  unsigned int addr = cpu->get_pc();
  const char *msg = fl_input("Your comment for address 0x%08x:", 0L, addr);
  if (msg) {
    FILE *f = fopen(fn, "ab");
    if (!f) {
      fl_message("Can't open the comment file. Comment is lost.");
      return;
    }
    ::fprintf(f, "c %08x %s\n", addr, msg);
    fclose(f);
  }
}


/**
 * This is our main entry point. We will try to restore as much of our
 * previous setup as possible. If loading any of the components fails, 
 * a dialog will pop up, asking the user to correct the settings.
 */
int main(int argc, char **argv) {

  // make this look good
  Fl::scheme("gtk+");

  // get all our filenames and settings
  load_preferences();

  // create the basic UI
  Fl_Window *win = create_gui();
  win->show(argc, argv);
  Fl::flush();

  // set up all memory that can be addressed by the CPU
  mem = new Dn_Memory();

  create_ROM_chunk();
  create_screen_chunk();
  create_RAM_chunk();
  create_flash_chunk();
  create_GPIO_chunk();

  // now set up the cpu itself
  cpu = new Dn_CPU(mem);

  // reas all the additional information that we can extract 
  // from various source
  read_symbols();

  // connect the UI with the cpu and our memory map
  wAsmView->cpu(cpu);
  wTracebackView->cpu(cpu);
  wTracebackView->set_traceback();
  wHexView->memory(mem);
  wRegisterView->cpu(cpu);

  // leave the rest to the User
  int ret = Fl::run();

  // save the contents of certain databases if asked to
  if (wSaveRAM->value()) {
    cRAM->save(wFilenameRAM->value());
  }
  if (wSaveFlash->value()) {
    cFlash->save(wFilenameFlash->value());
  }

  return 0;
}


