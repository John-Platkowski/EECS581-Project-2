**System Architecture**

**Purpose:** Describes the high-level structure of the Minesweeper implementation, including the timer, timed gameplay, and AI move-selection strategies added by the Project 2 Team.

**Components:**

- **Frontend (Qt GUI):** Provides the visible Minesweeper interface and receives user input through mouse interactions. Displays the board, cell states, game information, timer, and other UI elements. Uses the game logic to respond to player actions and display updated game states.
- **Game Logic (`LogicHandler`):** Acts as the primary controller for Minesweeper behavior. Receives user actions from the GUI, determines how each action affects the game, and manages revealing, flagging, mine detection, win/loss conditions, and board updates. It also owns the `GameTimer`, configures timed gameplay, handles time expiration, and provides three AI difficulty strategies that return the coordinates and click type of a suggested move.
- **Board (`Board`):** Represents the Minesweeper board and manages the collection of cells that make up the game grid. Provides access to cells, board dimensions, coordinate validation, and mine-related information.
- **Cell (`Cell`):** Represents an individual square on the board and stores its current state, including whether it is covered, uncovered, or flagged. Cells also maintain or provide access to mine information and the number of adjacent mines.
- **Game Timer (`GameTimer`):** Manages the game's time using Qt's `QTimer`. Supports both count-up and countdown modes, tracks the current time, and emits a `timeChanged` signal when the displayed time should be updated. It also supports adjusting the timer when squares are revealed or flags are placed correctly or incorrectly.
- **Game State / Display:** The logic layer maintains the current game state and communicates relevant board and game information to the GUI. The interface displays updated cell states, game outcomes, and timer information.
- **AI Move Selection:** Implemented within `LogicHandler`, the three difficulty strategies analyze the board and return a recommended row, column, and click type. Difficulty One selects a random covered cell. Difficulty Two applies basic rules based on uncovered cells and their adjacent mines and flags, falling back to a random move when no rule applies. Difficulty Three adds more advanced local pattern-based rules, including a 1-2-1-related heuristic, and also falls back to a random move when no rule applies.

**Data Flow:**

- User input (mouse click) → Qt GUI → `LogicHandler`.
- `LogicHandler` evaluates the action based on the current game state and the selected cell.
- `LogicHandler` → `Board` → individual `Cell` objects as needed.
- Updated board and cell states → `LogicHandler` → Qt GUI.
- The Qt GUI updates the board and displays the resulting game state to the user.
- The first valid move triggers mine placement while keeping the selected cell and its eight neighboring cells free of mines, providing a safe opening.
- `LogicHandler` manages the `GameTimer` by starting, stopping, resetting, and configuring it as required. The timer updates once per second and emits `timeChanged` to notify connected components of time changes.
- In timed mode, `GameTimer` tracks the countdown. When `LogicHandler::timeExpired()` is invoked, the game state changes to `Lost`, the timer stops, and all mines are revealed.
- Timer adjustments can add 15 seconds when a square is revealed, add 10 seconds for a correct flag, or subtract 5 seconds for an incorrect flag.
- For AI-assisted moves, a difficulty strategy examines the board and returns a tuple containing the target row, target column, and `ClickType` (`Left` or `Right`). The selected move can then be processed through the game's click-handling logic.

**Key Data Structures:**

- **`Board`:** Represents the game board and contains the collection of `Cell` objects used during gameplay.
- **`Cell`:** Represents one square on the board and stores its current state and mine-related information.
- **`CellState`:** Defines the possible states of a cell:
  - `COVERED` = Cell has not been revealed.
  - `UNCOVERED` = Cell has been revealed.
  - `FLAGGED` = Cell has been marked by the player.
- **Mine Information:** The board maintains information about which cells contain mines.
- **Adjacent Mine Count:** Each cell can store or use the number of mines in its surrounding cells to determine what should be displayed when uncovered.
- **Game State (`GameState`):** Tracks whether the game is currently `Playing`, `Won`, or `Lost`.
- **Click Type (`ClickType`):** Distinguishes between left-click and right-click actions. AI move-selection functions use this information to indicate whether a cell should be revealed or flagged.
- **Changed Cells (`CellList`):** Used by the game logic to track cells affected by operations such as revealing connected cells or revealing all mines.
- **Game Timer (`GameTimer`):** Maintains the current elapsed time or remaining countdown time. Its main data includes a Qt `QTimer`, `elapsedTime`, a countdown-mode flag, and the countdown's starting time.
- **AI Move Representation:** A tuple of `(row, column, ClickType)` represents an AI-selected move. The AI strategies use the board's current cell states, adjacent mine counts, and neighboring flags to select moves.
- **Random Number Generator (`std::mt19937`):** Used by `LogicHandler` to generate random mine layouts and select random AI moves.

**Assumptions:**

- The game uses a fixed **10×10 grid**.
- The number of mines is selected when starting a game.
- The first player move is guaranteed to be safe, including the eight neighboring cells.
- Left-clicking a covered cell attempts to reveal it.
- Right-clicking a cell toggles its flag state.
- Revealing a cell with zero adjacent mines recursively reveals appropriate neighboring cells.
- The GUI is responsible for displaying the current game state, while `LogicHandler` is responsible for determining the game's behavior.
- The timer supports both count-up and countdown modes, with the countdown starting from a configured time limit.
- Timer adjustments can modify the current time when cells are revealed or flags are placed correctly or incorrectly.
- The game can be lost by revealing a mine or through timed-mode expiration. A win occurs when all non-mine cells have been uncovered.
- The AI difficulty strategies select moves based on the current board state. Difficulty One uses random selection, Difficulty Two applies basic adjacent-cell rules, and Difficulty Three applies additional pattern-based rules before falling back to random selection.