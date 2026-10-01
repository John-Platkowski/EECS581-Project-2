**System Architecture**

**Purpose:** Describes the high-level structure of the Minesweeper implementation to facilitate feature extensions by the Project 2 Team.

**Components:**

* **Frontend (Qt GUI):** Provides the visible Minesweeper interface and receives user input through mouse interactions. Displays the board, cell states, game information, and other UI elements.
* **Game Logic (`LogicHandler`):** Acts as the primary controller for Minesweeper behavior. Receives user actions from the GUI, determines how the action should affect the game, and manages revealing, flagging, mine detection, and win/loss conditions.
* **Board (`Board`):** Represents the Minesweeper board and manages the collection of cells that make up the game grid.
* **Cell (`Cell`):** Represents an individual square on the board and stores its current state, including whether it is covered, uncovered, or flagged.
* **Game State / Display:** The logic layer communicates updated cell and game states back to the GUI, which uses those states to update what the player sees.

**Data Flow:**

* User input (mouse click) → Qt GUI → `LogicHandler`
* `LogicHandler` determines the appropriate action based on the input and current cell state.
* `LogicHandler` → `Board` → individual `Cell` objects as needed.
* Updated board and cell state → `LogicHandler` → Qt GUI.
* Qt GUI updates the board and displays the resulting game state to the user.
* The first valid move triggers the necessary mine placement before normal gameplay continues.

**Key Data Structures:**

* **`Board`:** Represents the game board and contains the collection of `Cell` objects used during gameplay.
* **`Cell`:** Represents one square on the Minesweeper board and stores its current state.
* **`CellState`:** Defines the possible states of a cell:

  * `COVERED` = Cell has not been revealed.
  * `UNCOVERED` = Cell has been revealed.
  * `FLAGGED` = Cell has been marked by the player.
* **Mine Information:** The board maintains information about which cells contain mines.
* **Adjacent Mine Count:** Each cell can store or use the number of mines in its surrounding cells to determine what should be displayed when uncovered.
* **Game State:** The game logic determines whether the game is still running, won, or lost based on the current board.

**Assumptions:**

* The game uses a fixed **10×10 grid**.
* The number of mines is selected when starting a game.
* The first player move is guaranteed to be safe.
* Left-clicking a covered cell attempts to reveal it.
* Right-clicking a cell toggles its flag state.
* Revealing a cell with zero adjacent mines recursively reveals appropriate neighboring cells.
* The GUI is responsible for displaying the current game state, while `LogicHandler` is responsible for determining the game's behavior.
