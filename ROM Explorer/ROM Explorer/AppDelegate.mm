//
//  AppDelegate.m
//  ROM Explorer
//
//  Created by Steven Frank on 5/28/15.
//  Copyright (c) 2015 Steven Frank. All rights reserved.
//

#import "AppDelegate.h"

#include "db2src.h"

#define ROM_PATH_KEY @"ROMPath"

@interface AppDelegate ()

@property (weak) IBOutlet NSWindow *window;
@end

@implementation AppDelegate

- (void)applicationDidFinishLaunching:(NSNotification *)aNotification
{
	NSString *romPath = [[NSUserDefaults standardUserDefaults] objectForKey:ROM_PATH_KEY];
	
	if ( romPath == nil )
	{
		NSOpenPanel *openPanel = [NSOpenPanel openPanel];
		[openPanel setTitle:@"Please locate the 717006 ROM file"];
		NSModalResponse response = [openPanel runModal];
		
		if ( response == NSModalResponseOK )
		{
			romPath = [[openPanel URL] path];
			
			[[NSUserDefaults standardUserDefaults] setObject:romPath forKey:ROM_PATH_KEY];
			[[NSUserDefaults standardUserDefaults] synchronize];
		}
	}
	
	if ( romPath == nil )
	{
		[NSApp terminate:self];
		return;
	}
	
	[self performSelector:@selector(loadROMAtPath:) withObject:romPath afterDelay:0];
}


- (void)applicationWillTerminate:(NSNotification *)aNotification
{
}


- (void)loadROMAtPath:(NSString *)romPath
{
	FILE *rom = fopen([romPath fileSystemRepresentation], "rb");
	if ( !rom )
	{
		NSLog(@"ERROR: Can't read ROM!");
		return;
	}
	
	fread(ROM, 0x00800000, 1, rom);
	fclose(rom);
	
	NSString *cppSymbols717006Path = [[NSBundle mainBundle] pathForResource:@"717006.cppsymbols" ofType:@"txt"];
	NSString *symbols717006Path = [[NSBundle mainBundle] pathForResource:@"717006.symbols" ofType:@"txt"];
	readSymbols([cppSymbols717006Path fileSystemRepresentation], [symbols717006Path fileSystemRepresentation]);
	
	zeroFlags();

	NSString *symbolsPath = [[[[NSBundle mainBundle] pathForResource:@"symbols" ofType:@"txt"] stringByDeletingLastPathComponent] stringByAppendingString:@"/"];

	load_db([symbolsPath fileSystemRepresentation], "symbols.txt");
	preset_rom_use();
	
	checkCodeCoverage();
	checkScriptCoverage();

	printf("\n====> Writing Newton ROM in ARM assembler code\n\n");
	unsigned int i;

//	strcpy(filename_buffer, base_path);
//	strcat(filename_buffer, "NewtonOS/newtonos.s");
//	FILE *newt = fopen(filename_buffer, "wb");
//	if (!newt) {
//		perror("Can't write NewtonOS!");
//	} else {
	FILE *newt = stdout;
	
	rom_flags_type(0x003AE204, flags_type_arm_word); // will create illegal instructions!
	rom_flags_type(0x003AE20C, flags_type_arm_word);

	unsigned int n_unused = 0, unused_filler = 0;

	//fprintf(newt, "\n\t.include\t\"macros.s\"\n\n\t.text\n\t.org\t0\n\n");
	//fprintf(newt, "\t.global\t_start\n_start:\n");
	
	for (i=0; i<0x00800000; i+=4)
	{
		unsigned int val = rom_w(i);
		
		writeLabel(newt, i);
		writeComments(newt, i);
		
		if ( rom_flags_is_set(i, flags_include) )
		{
			const char *sym = 0L;
			sym = get_plain_symbol_at(i);
			if (sym)
				AsmPrintf(newt, "\t.include \"%s.s\"\n", sym);
		}
		
		switch (rom_flags_type(i))
		{
			case flags_type_unused:
				if (!n_unused) unused_filler = rom_w(i);
				n_unused++;
				if (  (i+4)>=0x00800000
						|| rom_flags_type(i+4)!=flags_type_unused
						|| rom_w(i+4)!=unused_filler)
				{
					AsmPrintf(newt, "\t.fill\t%d, %d, 0x%08X\n", n_unused, 4, unused_filler);
					n_unused = 0;
				}
				break;

			case flags_type_ns_ref:
				i = decodeNSRef(newt, i);
				break;

			case flags_type_ns_obj:
				i = decodeNSObj(newt, i);
				break;

			case flags_type_arm_code:
			{
				AlData *d = gMemoryMap.find(i);
				if (d)
					d->exportAsm(newt);
				char buf[4096];
				disarm(buf, i, rom_w(i));
				char *cmt = strchr(buf, ';');
				if (cmt)
					*cmt = '@';
				AsmPrintf(newt, "\t%s\n", buf);
				break;
			}

			case flags_type_arm_text:
			{
				int n = 0;
				writeLabelIfNone(newt, i);
				AsmPrintf(newt, "\t.ascii\t\"");
				i -= 4;
				do
				{
					i += 4; n++;
					AsmPrintf(newt, "%s", p_ascii(ROM[i]));
					AsmPrintf(newt, "%s", p_ascii(ROM[i+1]));
					AsmPrintf(newt, "%s", p_ascii(ROM[i+2]));
					AsmPrintf(newt, "%s", p_ascii(ROM[i+3]));
				} while ( rom_flags_type(i+4)==flags_type_arm_text
								&& ROM[i] && ROM[i+1] && ROM[i+2] && ROM[i+3]
								&& !hasLabel(i+4)
								&& n<16);
				
				AsmPrintf(newt, "\"\n");
				break;
			}
			
			case flags_type_patch_table:
			case flags_type_jump_table: // TODO: these are jump tables! Find out how to calculate the offsets!
			case flags_type_arm_byte:   // TODO: currently not used
			case flags_type_rex:        // TODO: interprete the contents
			case flags_type_ns:
			case flags_type_dict:
			case flags_type_classinfo:  // TODO: differentiate this
				AsmPrintf(newt, "\t.word\t0x%08X\t@ 0x%08X \"%c%c%c%c\" %d (%s)\n", val, i,
				printable(ROM[i]), printable(ROM[i+1]), printable(ROM[i+2]), printable(ROM[i+3]),
				rom_w(i), type_lut[rom_flags_type(i)]);
				break;

			case flags_type_data:
			case flags_type_unknown:
			case flags_type_arm_word:
			default:
			{
				// if it matches a label, is it a pointer?
				// if all bytes are ASCII, is it an ID?
				// if it is a negative number, is it an error message?
				const char *sym = 0L;
				if (val)
					sym = get_plain_symbol_at(val);
				if (!sym)
					sym = "";
				AsmPrintf(newt, "\t.word\t0x%08X\t@ 0x%08X \"%c%c%c%c\" %d (%s) %s?\n", val, i, printable(ROM[i]), printable(ROM[i+1]), printable(ROM[i+2]), printable(ROM[i+3]), rom_w(i), type_lut[rom_flags_type(i)], sym);
			}
				break;
		}
	}
	
	// write all symbols that are not within the ROM area, but are called from ROM
	//fprintf(newt, "\n\n@\n@ Symbols outside of the ROM\n@\n\n");
	//AlMemoryMapIterator it(gMemoryMap);
	//while (!it.end()) {
	//	unsigned int addr = it.address();
	//	if (addr>=0x00800000) { // beyond ROM
	//	AlData *s = it.data();
	//	const char *sym = s->label();
	//	if (sym) {
	//	// FIXME: cpp support
	//	// const char *cppsym = s->decodedName();
	//	// if (cppsym)
	//	// fprintf(newt, "\t.equ\tVEC_%s, _start+0x%08X\t@ %s\n", sym, addr, cppsym);
	//	// else
	//	// fprintf(newt, "\t.equ\tVEC_%s, _start+0x%08X\n", sym, addr);
	//
	//	fprintf(newt, "\t.equ\tVEC_%s, _start+0x%08X\n", sym, addr);
	//	}
	//	}
	//	it.incr();
	//	}
	//
	//	fprintf(newt, "\n\t.end\n\n");
	//	fclose(newt);
	//	}
	printf("\n====> DONE\n\n");
}

@end
