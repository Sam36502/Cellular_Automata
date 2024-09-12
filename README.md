# Cellular Automata

My experiments with writing basic cellular automata
using SDL2 to display the graphics.


## How to use

When you run `cells.exe` or build/run the project with `make run`,
a small window will open with all the pixels randomised.
The controls are as follows:

 - **Enter**: Play/Pause the simulation
 - **Backspace**: Reset the cells (randomises the pixels)
 - **Keypad Plus**: Increase "temperature" (explained below)
 - **Keypad Minus**: Decrease "temperature" (explained below)
 - **Mousewheel Scroll**: In-/Decrease brush size
 - **Left Click**: Paint the selected region with cell type 1

The cellular automaton implemented here at the moment is one
based on the "Rock Paper Scissors" CA demonstrated in
[this video](https://www.youtube.com/watch?v=M4cV0nCIZoc).
I've expanded it to include more cell types that still follow
a cyclical hierarchy, i.e.

	Red -> Orange -> Yellow -> ... -> Magenta -> Red

The "temperature" variable (I named it that because of the visual effect it has)
determines how many steps down the hierarchy each cell type looks
when checking if it can replace a given cell.
With a value of zero, no cells can replace any others and the simulation "freezes".
For one, red cells can *only* take over orange cells, which can only take over
yellow cells, and so on. For two (the default), red cells can replace orange
cells, but also yellow ones.


## Future Development

At the moment, I'm just uploading this to have it available on my GitHub,
but having looked at it again, and gained knowledge of GPU programming,
I have many ideas I'd like to implement if I get the time:

 - Implement Simple GUI to make it easier to use
 - Expand it to more types of cellular automata (e.g. Conway's Life)
 - Possibly do the above using a meta language like ALPACA
 - Add Hardware-Accelerated rendering support
 - Add more customisability options (colour palettes)
 - Add ability to record and save GIFs directly from the program
 - Expand "drawing" capabilites