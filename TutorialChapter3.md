Thanks to Matt Kow and Frank Gruendel for their help and suggestions.

# Introduction #

We will write our very first program today. No worries, this is a very simple program, and DyneTK will take over most of the work. It's almost entirely click and drag.

This tutorial requires DyneTK 1.7.1, a Newton with a serial communication cable or Einstein, and a PC or Mac. You should have connected your Newton as discussed in [part one](TutorialChapter1.md) of the tutorial.


# Creating a new Project #

Let's start todays application by creating a [Project File](TutorialGlossary#Project_File.md). This file contains very basic information, like the name of our app, and holds all the strings to the various resources which we will create later.

DyneTK project files are compatible to MS Windows NTK project files and use the file extension **.ntk**.

Select **Project > New Project** in the main menu. A file chooser will pop up, asking you for the name of the project file which DyneTK will create later. Set the filename to something you like, for example **hello.ntk**. DyneTK will create all standard setting for an application by the name **hello**.

A lot of new menu items and icons have become active (on a side note, blue menu items are actions that exist in NTK, but are not implemented in DyneTK yet). Select **Project > Settings...** to get a glance at some of the presets. There is no need to change anything here, so close the dialog for now.

![http://weinbrennerei-dujardin.de/Newton/Tutorials/t3projectsettings.jpg](http://weinbrennerei-dujardin.de/Newton/Tutorials/t3projectsettings.jpg)


# Creating a Layout #

Now we need to create a [Layout](TutorialGlossary#Layout.md). The layout will describe all visual elements of our user interface and more. It can be created by writing lots of lines of code in a text editor, which is error prone and boring. DyneTK comes with a visual editor that makes assembling a user interface much easier.

DyneTK layout files again are compatible to MS Windows NTK layout files and like to have the file extension **.lyt**.

Select **File > New Layout**. A new layout document will appear, creatively named **layout.lyt**. To make the new layout a part of our project, select **Project > Add Window**. A file chooser will pop open and demand a better name for our new layout file. How about **hello.lyt**? It is useful to keep the layout files in the same directory as the project files so they can be easily found when reopening the project again later.

![http://weinbrennerei-dujardin.de/Newton/Tutorials/t3hellolyt.jpg](http://weinbrennerei-dujardin.de/Newton/Tutorials/t3hellolyt.jpg)

The **Files** list on the left of DyneTK shows our first entry **`* hello.lyt`** and the label of the layout tab changed to **hello.lyt** as well. The asterisk in front of the list entry tells us, that this layout is the main layout of our application.


# Create the Application View #

When the Newton launches an application, it opens the application Main [View](TutorialGlossary#View.md). A view is just any kind of user interface element, sometimes containing a whole bunch of more elements. On a desktop computer, an example would be a window with a button in it (which incidentally is exactly what we are going to create now).

A View is created from a [Template](TutorialGlossary#Template.md). This may sound complicated, so let's just ignore my gibberish for now and go an create this Template thingie:

![http://weinbrennerei-dujardin.de/Newton/Tutorials/t3addproto.jpg](http://weinbrennerei-dujardin.de/Newton/Tutorials/t3addproto.jpg)

Select **protoDragNGo** from the **Selection drop list** (you may have to wiggle the mouse a bit to get to the bottom of that menu). The Layout window will appear and the **Create Proto** button will be activated. Create the Drag'n'go Proto Template by clicking into the Layout window and dragging out a rectangle.

Oh no, more gibberish:
  * a _Proto_ is a prototype for a user interface element, provided by NewtonOS for your disposal (a push button, a menu, a window, etc.)
  * a _Template_ is what you make out of a Proto by adding functionality (a push button that pops up a dialog, a menu which lets the user choose red or green, a window which has a light gray background)
  * a _View_ is what NewtonOS creates from your Template when the app launches, the live version of your template.
  * a _Layout_ is a hierarchy of Templates and their new functionality. Many programs can be written entirely as a single layout.


# Build the Application #

Have I mentioned that you just wrote a brand new application? Well, let's try to build a package then. Select **Project > Build Package**. The Inspector should show:

```
Building package
Package build successfully (3kBytes)
Writing package to /somewhere/hello.pkg
Done.
```

Two things I find amazing about developing for the Newton: it took us a few seconds of clicking to create a new application setup (if you program in C++ you will know why I find this amazing). And the resulting package is 2968 bytes small, including the application icon graphics and not using any compression.

# Run the Application #

I mentioned earlier that you don't need an Inspector connection for most of the tutorial. So if you are using Einstein, go ahead and drag **hello.pkg** onto the emulator. You will find your app in the **Extras** folder in **unfiled icons** named **hello**.

If you _do_ have a Newton hooked up, start the Inspector now. Select **Project > Download Package** to copy the package from your PC to the Newton, then select **Project > Launch Package**. True programmers are lazy, so we put these reoccurring steps on the shortcut keys **Ctrl-1**, **Ctrl-2**, and **Ctrl-3** (OS X users press **Cmd-1**, **Cmd-2**, and then **Cmd-3**).

An empty draggable window with a closing button will appear on your Newton. SO now we know why this proto is called a DragNGo: drag it around and make it go away.

Congratulations, you just wrote your first Newton application!


# Adding Functionality #

We started this tutorial wanting to greet the world, s let's go and add that functionality to our app.

![http://weinbrennerei-dujardin.de/Newton/Tutorials/t3templatelist.jpg](http://weinbrennerei-dujardin.de/Newton/Tutorials/t3templatelist.jpg)

Select the **protoDragNGo** by either clicking into the Template list or by clicking into the Layout window. Select **protoTextButton** from the **Selection drop list** and drag out a button somewhere inside your drag'n'go.

Select the text button. The Slot list provides a selection of Methods and Attributes for this button.

Gibberish:
  * a _Slot_ is a description of any kind of property a view may have. A Slot may describe a simple property like the background color of the button (viewFormat), or very complex properties, for example a database query system whenever the button is tapped by the user.
  * a _Method_ is a slot that describes what a View can do. It contains an action or subroutine. These methods are executed whenever something happens to the view.
  * an _Attribute_ is a slot that describes what a slot is, how it looks, and where it is positioned. These slots usually contain static values.

![http://weinbrennerei-dujardin.de/Newton/Tutorials/t3buttonclickscript.jpg](http://weinbrennerei-dujardin.de/Newton/Tutorials/t3buttonclickscript.jpg)

Select the Slot named **buttonClickScript** (if there is no such slot, you have not selected the text button yet). The slot editor will show the words

```
func()
begin
end
```

Hmm, so whenever the user taps (clicks) the button, this code will run. Do you still remember the line of code that we sent to the Newton in the second tutorial? Let's enter this here and see what happens

```
func()
begin
  AlarmUser("DyneTK", "Hello, World!");
end
```

Build and run (**Ctrl-1**, **Ctrl-2**, **Ctrl-3**), then tap the "Button" on your Newton. Does your screen look somewhat like this (no, the tutorial is not from June 1996)?

![http://weinbrennerei-dujardin.de/Newton/Tutorials/t3hellonewt.jpg](http://weinbrennerei-dujardin.de/Newton/Tutorials/t3hellonewt.jpg)

You are now ready for Tutorial 4 which will add some new views and some interaction.

# One more thing... #

To create screenshots from you Newton, select **Edit > Newt Screen Shot** while connected via Inspector. The download will take a little while, but it is still a lot faster than taking a screenshot with your 1996 Polaroid camera... .