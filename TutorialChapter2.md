# The Inspector #

The Inspector is your direct link right into your Newton via the serial cable. You can use the Inspector to look at the internal structures of your Newton, run NewtonScript commands, and to download, launch, and debug your applications.

It is possible to write NewtonScript apps without the help of Inspector by uploading packages manually. This is currently the only way for Einstein users.


## Setting up the Serial Port Connection ##

If you have not done so yet, connect your Newton via the serial cable to your serial port or serial port USB dongle. Make sure that all connectors fit snug.

Install the **Toolkit.pkg** on your Newton device. You find this package on http://www.unna.org/ as part of the original NTK, but I also included it in the program folder (MS Windows) or Disk Image (1.7.1 and up).

![http://weinbrennerei-dujardin.de/Newton/Tutorials/t2connection.jpg](http://weinbrennerei-dujardin.de/Newton/Tutorials/t2connection.jpg)

Start DyneTK and select **Edit > Toolkit Preferences...** from the main menu. In the **Packages** tab and set **Port** to whichever serial port you are using. On MS Windows, this should be something like **\\.\COMn** (the \\.\ are needed for port numbers n = 10 and above). On a Mac, this will likely be **/dev/tty._something_**, where "something" depends on the manufacturer of your USB dongle.

Close the preferences dialog. You only have to do this once.


## Connecting to the Newton ##

On your Newton, open the **Extras** folder and launch **Toolkit**. Choose **Connect Via Serial** from the menu and tap on **Connect Inspector**.

![http://weinbrennerei-dujardin.de/Newton/Tutorials/t2toolkit.jpg](http://weinbrennerei-dujardin.de/Newton/Tutorials/t2toolkit.jpg)

In DyneTK, the Inspector is located in the lower third of the main window in the first register tab. Right under the word "Inspector" is a button with a red toolbox. Click the toolbox to connect to the Newton. A **Progress** dialog box will pop open.

![http://weinbrennerei-dujardin.de/Newton/Tutorials/t2inspector.jpg](http://weinbrennerei-dujardin.de/Newton/Tutorials/t2inspector.jpg)

<sup>An open toolbox indicates a successful connection. The activity light will blink every few seconds, indicating a live connection.</sup>

If your serial connection is working correctly, the Toolkit window will disappear, and so will the Progress dialog. The DyneTK Toolbox is now open and the Inspector buttons become active. The green indicators on the right of the Inspector will flash red every few seconds or so. Great! You are connected!

To close the connection, either click on the DyneTK Toolbox button again, or tap the Toolbox on the Newton, then tap **Disconnect Inspector**.


## Putting the Newton to Work ##

Let's find out if we really are connected to the Newton. Click into the Inspector text window and type this on a fresh line:

```
print(1+2);
```

Then press _Enter_ on the numeric keypad (yes, this is one of those old Apple style conventions. Hitting _Return_ will merely bring the cursor into the next line, but only _Enter_ will run your code. MS Windows users may instead press _Ctrl-Enter_. Mac users may find the _Enter_ key to the right of the space bar if they don't have a numeric keypad - upon multiple requests I will break with this tradition in one of the next releases of DyneTK).

After a second or so, Inspector should spit out:

```
3
#2   NIL
```

Whoo-hee! Dora, Boots, we did it.


## Could it be my Newton? ##

Oh, so you think this is an evil trick and all those flashy lights are here to distract you and you PC calculated 1+2, not your Newton? OK, let's prove that the Newton is at your command. Type:

```
AlarmUser("DyneTK", "All your Newts are belong to us!");
```

Press _Enter_ (not _Return_). You should get a very informative dialog box, popping up on your green screen.

## Next! ##

If you made it through this, you may pat yourself on the shoulder. You just mastered what may be the hardest part of the tutorial: getting your PC and your PDA to communicate.

Now you are ready for [Chapter 3](TutorialChapter3.md), Hello Newton!