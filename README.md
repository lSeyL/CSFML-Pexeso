 Pexeso (Memory) Game

Pexeso is a classic memory card-matching game implemented in **C** using **CSFML** (Simple and Fast Multimedia Library for C). This version includes **network functionality**, allowing players to compete with each other using sockets for communication.

---

## 🎮 Features

- **Single-player Mode**: Practice and improve your memory.
- **Multiplayer Mode**: Challenge your friends via network play.
- **Customizable Board**: Adjust the difficulty with different grid sizes.
- **Socket Communication**: Seamless networking using sockets for multiplayer functionality.

---

## 🚀 Getting Started

Follow these steps to set up and run the game on your machine.

### Prerequisites

- **CSFML Library**: Install the CSFML library ([Official Site](https://www.sfml-dev.org/download/csfml/)).
- **C Compiler**: Ensure you have GCC or any compatible compiler installed.
- **Socket Support**: Ensure your system supports socket programming.

### Installation

1. **Clone the Repository**:
   ```bash
   git clone https://github.com/your-username/pexeso-game.git
   cd pexeso-game
   ```

2. **Build the Project**:
   - Use mingw64 with CMakeLists.txt or the provided `Makefile`:
     ```bash
     make /all
     ```
    - Or :
     ```bash
     make client
     ```
     ```bash
     make server
     ```
3. **Run the Game**:
   ```bash
   ./pexeso
   ```

---

## 🕹️ How to Play

### Single-player Mode
1. Start the game and select **Singleplayer**.
2. Flip two cards at a time to find matching pairs.
3. Select difficulty for the bot, and choose between Free mode and Timed mode.
4. Match all pairs to win.

### Multiplayer Mode
1. Start the game and select **Multiplayer**.
2. Choose to host a game or join an existing one.
3. Take turns with your opponent to find matching pairs.

---

## ⚙️ Networking Details

- **Protocol**: TCP sockets for reliable communication.
- **Host Setup**: The host player creates a socket server.
- **Client Connection**: Other players connect using the host's IP address.

---

## 📁 File Structure

```
Pexeso/
├── src/                # Source code files
├── Resources/          # Game assets (.png files)
├── Makefile            # Build script
└── CMakelists.txt      # Linker
```

---

## 🛠️ Future Improvements

- Add more themes and card designs.

## 📧 Contact

- **Author**: Martin Grísa
- **Email**: martinngrisa@gmail.com
- **GitHub**: [lSeyL](https://github.com/lSeyL)

Feel free to reach out if you have questions or suggestions!
