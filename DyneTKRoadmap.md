# Roadmap #

In it's current state DyneTK is capable of creating, compiling, and downloading fully functional Newton applications. User Protos are not yet implemented. Many little features are still missing, and there has been no QA yet.

This is a list of tasks, somewhat in the order of implementation:

  * finish reading layout files (PC format)
  * finish reading layout files (Mac format)
  * grids and snapping for layout views
  * missing editors
    * viewFormat
    * viewFlags
    * viewTransferMode
    * viewEffect
    * boolean
    * picture
  * missing menu items
    * printing
    * all edit (specifically: undo/redo!)
    * document order
    * all layout
    * all browser
    * all help
  * missing dialogs
    * Rename Slot (minimal implementation)
    * Layout Size
    * Set Grid
    * Alignment
    * Link Layout
    * Search
    * Find
    * many controls in project and application settings dialogs
  * improvements to compiler
    * better error messages, opening the corresponding slot
    * better exception handling, again with giving more of a clue where the error occured
  * improvement to the inspector
    * Replace Ctrl-Enter with Enter, and Enter with Shift-Enter functionality
    * make the tools work correctly (toggle "break on execption", etc.)
    * output the correct stack dump when handling an exception
    * more debugging capabilities
  * compile time functions
    * DefineGlobalConstant(symbol, expr)
    * UndefineGlobalConstant(symbol)
    * IsGlobalConstant(symbol)
    * GetLayout(filename)
    * SetPartFrameSlot(slot, value)
    * GetPartFrameSlot(slot)
    * Load(pathname)
    * ReadStreamFile(pathname)
    * GetSoundFrame
    * MakeDitheredPattern
    * MakeExtrasIcons
    * MakePixFamily
    * UnpackRGB
  * improve visual layout editor
  * import of image and sound data
  * writing of Mac style project files
  * writing of Mac style layout files
  * documentation