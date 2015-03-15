_You will need DyneTK 1.7.3 or better to complete this tutorial.
MPG is part of the download (mpg.zip)_

_I did not have the time to do screenshots yet. I hope this will work for you regardless_

_This program uses miles, gallons, and miles per gallon as units. I leave the metric conversion (km, l, l/100km) as homework_ ;-)

## What is MPG? ##

I decided to develop an application over the next few tutorials which I will call MPG. MPG starts out very simple doing some conversions. I will add the ability to log fill-ups (_Soups_), calculate monthly cost (_Preferences_) based on driving efficiency, and eventually draw some statistics (_Drawing_) and suggest ways to save money (_Data Transfer_).

I chose this project because it will use the most basic as well as advanced aspects of NewtonScript programming, basic math, soup access, custom graphics, and eventually communication, in a nice incremental fashion. The final product can be useful to most of us who drive, and if you don't own a car, you can still use the app to find out just how much money you save every month... .

## Create a New Project ##

Create the usual project as described in the previous chapter:
  * select **Project > New Project...**, choose a directory and call the project _mpg_
  * select **File > New Layout** to start a new layout
  * next, select **Project > Add Window...** to add a new layout. I named it _mpg.lyt_, so I know which files belong together

## Create the Main Window ##

Choose **protoDragNGo** in the _Create Proto_ menu. The layout view should pop up. I like to use the DragNGo because the app can be repositioned on the screen. You could also use a _protoApp_ for a more complete interface, or _clView_ for a minimal interface. My DragNGo is about 200x150 pixel large.

Let's give this new view a name. Make sure the DragNGo is active and select **Browser > Template Info...**. A dialog pops up with a _name_ field and a pulldown menu. Enter _wBase_ for a name and leave the menu alone. OK. You can choose almost any name, starting with a letter (a-z, A-Z), followed by letters, numbers, or underscores. Other combinations are possible, but will make life much harder later on. I decided for _Base_ because this view is the base for all functions and elements, and prepended the letter 'w' to indicate a widget (user interface elements are often called widgets; this is my personal naming scheme, but you can choose pretty much any other name).

## Create the Numeric Input Fields ##

We will now create three number pickers from top to bottom. I chose pickers over input fields because they can be tapped with the finger. This may come in handy at the gas station where our rare and precious pens could drop into some greasy oil puddle.

Choose **protoNumberPicker** in the **Create Proto** menu. Position the picker anywhere inside _wBase_ (this position is not yet permanent). Activate the picker in the layout browser and open the Info browser by selecting **Browser > Template Info...**.Name it _wTrip_, and choose _wBase_ in the **Declare To** menu. _wBase_ can then directly access _wTrip_ when the program launches.

Now let's prepare the picker for entering the amount of miles driven between fuel stops. Keep _wTrip_ active and add the slot **viewBounds** from the **Specific** menu. Click **Apply** to transfer the coordinates. In the layout view, drag the picker a bit towards the top of _wBase_, so you have room for two more pickers. Keep the picker 24 pixels high.

We need to update a few more slots. Activate the _value_ slot in the slot browser and change the value from _nil_ to _0_ (zero). Then change the _maxValue_ slot to _9999_.

Allright, we should give this a test run. Connect your Newton or Einstein and compile, download, and run (Ctrl-1, Ctrl-2, Ctrl-3). Does the window pop up? Can you flip digits? I you get an error -48406, you probably missed to change the _value_  slot from _nil_ to the number 0 (I know that because I did it a few times myself ;-) ).

## More Numeric Pickers ##

The first field will be used to enter the miles between fill-ups. We need two more pickers, one for the amount of gas to top of the tank (I call it _wGas_, set _value_ to 0, and set _maxValue_ to _99_), and one to show the calculated gas mileage (I call that one _wMileage_, set _value_ to 99, and set _maxValue_ to _99_ as well. You can also use three digits if you expect to get very good gas mileage... ).

## Make the Pickers Do Something ##

Tapping _wTrip_ and _wGas_ shall cause a recalculation of the mileage. We will tell the Pickers to start an action whenever the user taps them. Add the slot **ClickDone** from the **Specifics** menu to both pickers and enter this text:

```
func()
begin
  :recalculateMileage();
end
```

Whenever the user taps one of these pickers, the script in _ClickDone_ is run. `recalculateMileage()` is a function that we will write in the next paragraph.

## Calculus ##

No worries, we won't diverge into higher math, but we will program our formula now. Whenever the user changes _wTrip_ or _wGas_, we want to recalculate the gas mileage and update _wMileage_. I mentioned earlier that _wBase_ has direct access to all three pickers, so _wBase_ is the perfect place for our calculations.

Activate _wBase_ and select **Browser > New Slot...**. A slot can be any kind of data, some of which you already used. A slot can contain the coordinates of a rectangle (_viewBounds_, for example), a numeric value (_maxValue_, _value_, etc.). We will create a slot named **recalculateMileage** and choose **Script** in the **Editor** pulldown. A script is a piece of program code that will be run whenever the script function is called.

A new slot _recalculateMileage_ pops up in the slot browser and a big white text field waits for our wise input. Let's enter our code line by line:
```
func()
```
tells the compiler that this is a (script) function.
```
begin
```
marks the beginning. I am sure you can guess what comes at the end  of the function?! Programmers will indent the following lines by a few spaces, so `begin}} and {{{end` become visually connected.
```
  local trip := wTrip.value;
  local gas := wGas.value;
  local mpg := wMileage.maxValue;
```
finally, after three and a half chapter of tutorials, or first true instruction! These lines declare _variables_, places in memory that will hold some values. We need these variables only while in the function, so we declare them `local`- they will be thrown away as son as we leave the funcition. Then we give the variables descriptive names: `trip`, `gas`, and `mpg`.

The `:=` copies the value from the right into the variable to the left. NewonScript uses the `:=` for asignments and single `=` for comparison. This is different to "C" and some other languages. Don't confuse these.

The right side of the assignment can be pretty much anything from a single numeric value to a complex mathematical calculation. In our example, we query the slot _value_ from the view _wTrip_. The semicolon indicates the end of the statement.

We set _mpg_ to the maximum possible value to indicate an invalid entry (if _wGas_ is set to 0, no mileage can be calculated). If on the other hand there is an amount of gas larger than zero, we override _mpg_ with the result of our scientifically proven formula for mpg: the length of the last trip divided by the gallons of gas we needed:
```
  if gas>0 then begin
    mpg := trip / gas;
  end;
```
The `if` statement protects the division of _trip_ by _gas_ from generating an error. If _gas_ is 0, the following statement will be skipped and _mpg_ will remain at _99_. The second line calculates the gas mileage using a simple division. The `end` is the counterpart to the `begin` statment after `then`.
```
SetValue(wMileage, ´value, floor(mpg));
wMileage:Dirty();
```
These two statements call other functions, or as they say in NewtonScript, send a message (because the internals are much more powerful than a simple call, but for now, we stick with the simple idea). `SetValue` is a function provided by the operating system which changes a value in a view. We could also just assign a value like we did in the first lines of this script, but `SetValue` makes sure that the picker itself will be drawn again to show the new value.

The method `SetValue` has three _arguments_. The first one describes the view that we want changed (our number picker _wMileage_). The second argument is the name of the slot that we change. If names of slots stand alone, they are marked by a single quote `'`, similar to text which is enclosed in double quotes `"text"`. The third argument is the new value. `floor` removes any fractional part from the mileage.
```
end
```
Here is the full function again:
```
func()
begin
  local trip := wTrip.value;
  local gas := wGas.value;
  local mpg := wMileage.maxValue;
  if gas>0 then begin
    mpg := trip / gas;
  end;
  SetValue(wMileage, ´value, floor(mpg));
  wMileage:Dirty();
end
```

Compile, download, and run. Clicking the trip or gas field should cause immediate recalculation of the mileage.

## Error Messages ##

I am sure that you will encounter some errors on the way. There are a bunch of different error types that can occur.

  * **syntax errors**: DyneTK takes all your layout, templates, and slots and converts them into a large text file. Obvious tpos and illegal constructs will create a syntax error during the build phase. These errors are shown in the Inspector window with a text like `line 1680: syntax error`. At this point, the line number won't help you much, but we are working on DyneTK to improve this flaw.
  * **compile-time errors**: after the syntax is checked, DyneTK builds a package from the instructions given in the above text file. These errors are usually caused by typos in slot names or other undifined constants. The also appear in the Inspector window, looking like this `**** ERROR while compiling or interpreting`. Again, DyneTK will become more verbose in the future.
  * **run time errors**; these are also called _exceptions_ and occur if you try to access something that doesn't exist or is not the expected type kind. These pop up on the Newton, declaring that _a problem has occured_, followed by a negative number. Enter that number in the DyneTK bug field to get a little bit more info. In 90% of all cases, these are typos... .

## Static Text ##

Now that we have created the functional part of our UI, we should label those number pickers, or nobody will know how to use our app. I suggest a titile line with the name of the app, MPG, a short description below "calculate your mileage", and description and unit around the number pickers ("trip: 999 mi", "Gas: 99 gal", "Mileage: 99 mpg".  Add new _protoStaticText_ slots to _wBase_ wherever needed. I know you will be able to that on your own now.

Matthias