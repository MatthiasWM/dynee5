Call me nuts (you wouldn't be the first one), but I spent quite a few hours in the last three months on developing an alternative Newton MP2100 emulator. It is called DyneE5 (Dyne Ehh Five), or Dyne for short (yeah yeah, I know).

I wasn't going to publish any code until the emulator would boot all the way up, but I am somewhat stuck at this point, and I am hoping to find the one or other developer interested in helping to find the bugs and explore.

Here it is:  http://code.google.com/p/dynee5/wiki/Featured

The source code is hidden in the SVN section. There is a minimal MSWindows executable in the download section which is mostly to show the UI, but won't do a thing without the ROM, obviously.

_- But why?_

Well, I like the Newton. By writing this emulator, I have learned a helluva lot about emulation, ARM, and the Newton OS itself. Other than that, this is purely for education and fun.

_- But you know that there is Einstein, right?_

Yes, I know. Einstein was the reason that I dug up my old MP and started playing with it again. Man, oh man, the response times of my 14 yr old Newton compared to my brand new PocketWindows PDA are incredible! Anyways, Einstein runs only on OS X (which I love, but it's on my slooow machine) and I can't see how it works. Dyne fixes this by compiling and running on OS X, Linux, and MSWindows, and it is OpenSource under GPL 2.

_- So what can Dyne do?_

Well, Dyne does emulate the StrongARM, the Newton LCD, some of the Interrupts, even hacking in a bit of a Digitizer Code. It can disassemble, single-step, handle breakpoints and symbolic information. It provides functions to output the contents of structs and classes, add code comments, and inject native code.

_- Great, but there is a catch, right?_

Sure. Dyne does run quite far into the boot process, and I managed to pop up the dialog boxes to erase all Flash (and did erase it). It is slow and still has some bugs that keep the OS from booting all the way through though. This is why I am asking another pair of eyes to look at the code. I would love to see a faster and cleaner StrongARM emulator added. Also, most of the Cirrus registers are not emulated. Please help if you can.

You must provide your own ROM and do this purely for educational purposes. Hey, we are not hacking or cracking anything here, we just want to get a glimpse into the genius of the first and best PDA OS ever.

Enjoy,

Matthias