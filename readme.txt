Win2DS v0.5 Source

By Bill Blaiklock (Sintax)
------------------

You are free to use aspects of this code in your own projects, but please give credit where credit is due.

The server source is in the Win2DS folder and the nds source is in the DS2Win folder inside of that. Some paths rely on how these folders are nested, and some paths are hardcoded into the makefiles, so be careful if trying to build this yourself. This source is lightly commented, but includes a GUI system I hoped would be able to be used by other people.


------

The GUI is used something like this:

*

//Load the text first
loadText((u16*)CHAR_BASE_BLOCK_SUB(0),(u16*)BG_PALETTE_SUB);


CGUI *GUI;

//parameteters: ((0)where to load the tiles, (1)where to use the tile map, (2)where text is loaded, (3)true if using the sub screen)

GUI = new CGUI((uint16*)(CHAR_BASE_BLOCK_SUB(0)),(uint16*)SCREEN_BASE_BLOCK_SUB(7), (uint16*)SCREEN_BASE_BLOCK_SUB(6), true);

*

This will load the tiles into tile slot 0, use tile map slot 7, use text (which needs to be loaded beforehand) in map slot 6, and true since it will be on the sub screen. After this you just need to reference GUI to create windows and windowObjects.

*
CWindow *mainWindow;
CButton *testButton;

mainWindow = GUI->createWindow(&WND_MAINMENU);	//WND_MAINMENU should be a struct filled with data to use to create it. Check the source for examples.
testButton = mainWindow->addButton(&BTN_TESTBUTTON);

*

This should be all you need to create a window with a button. Make sure you run a loop with GUI->processInput(); so that the window will recieve input. Make sure to use destroyWindow before creating another, or you could get problems (better to edit createWindow to destroy one if it is open). Check the source on how to recieve and handle states, the objects all use states, for example if a button is pressed it will be set to the state of BTN_ACTIVATED, you should manually reset this state if you wish to read it again. The window will recieve a state of WND_SEL if a selector is activated and has OK and CANCEL states that are set to default buttons START and B respectively.

The way I loaded things into VRAM is messy, so that may give you problems (I had to fit two screens worth of screenshots in there as well as GUI and text graphics, and I put them all into one bank since all the bank crap still confuses me), for instance the palette the GUI tiles use is hardcoded to use palette slot 3, check gui.cpp for more info. But I think it should work mostly as described.