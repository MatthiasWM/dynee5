# How To Build #

DyneTK can be built on OS X using XCode or command line, on Linux using the command line, and on MSWIndows using VisualC 2005 (V8) and probably above.

## Building DyneTK from the Command Line ##

First, we need to create a directory containing all required components. Let's call it "dev". Inside "dev", we create the subdirectories "dyne", "newt0", and "fltk" by checking out the following Subversion repositories:
```
> svn co http://dynee5.googlecode.com/svn/trunk dyne
> svn co svn://so-kukan.com/gnue/NEWT0/branches/DyneTK newt0
> svn co http://svn.easysw.com/public/fltk/fltk/branches/branch-1.1 fltk
```

First, we need to build and install FLTK:
```
> cd fltk
> autoconf
> ./configure --enable-threads --enable-debug
> make
> sudo make install
> cd ..
```

Next, we build Newt/0. You may have install the IConv libraries first. IConv is required to convert ASCII into UNicode 16bit wide character codes and back. Newton uses wide character strings for all text rendering.
```
> cd newt0
> touch contrib/inwt/inwt.in
> autoconf
> ./configure --enable-compat
> make libnewt
> cd ..
```

Finally, we can build DyneTK:
```
> cd dyne/DyneTK
> make
> ./dynetk
```