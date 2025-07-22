# Birthday Simulation

This is a TUI (Terminal User Interface) application written in C that shows how birthday paradox works, prove how birthday attack can finds collision in less than half of the hash space count.

## Getting Started

To run, build or develop the application, you should start from here.

### Library Used

- [OpenSSL](https://www.openssl.org/) - For generating cryptographically random bytes for hashing, which also provides the hash function the program needed like `sha256`, `sha512`, etc..
- [ncurses](https://invisible-island.net/ncurses/) - The core library to build UI in this CLI application

## Prerequisites

- If you are on Windows, you need to install [MSYS2](https://www.msys2.org/). That should also install **UCRT64** for you.
- Install all library listed in the [Library Used](#library-used) section. In windows, you should use the `UCRT64` terminal and pacman to search and install the library.
- Install `CMake` and `make`

### Build

1. Clone the repository into your local machine

   ```bash
   git clone https://github.com/AlstonChan/birthday-simulation.git
   ```

2. Generate the `make` file with `CMake`, for your platform:

   On Windows, using **MinGW**

   ```powershell
   cmake . -G "MinGW Makefiles"
   ```

   On Linux/Unix, using the standard unix makefile

   ```bash
   cmake . -G "Unix Makefiles"
   ```

3. Run `make` to build the application

   ```bash
   make
   ```

## Resources

- [Ncurses How to Guide](https://invisible-island.net/ncurses/howto/NCURSES-Programming-HOWTO.html)
- [Ncurses manual](https://invisible-island.net/ncurses/man/ncurses.3x.html)
- [Ncurses Menu manual](https://invisible-island.net/ncurses/man/menu.3x.html)
- [Ncurses Form manual](https://invisible-island.net/ncurses/man/form.3x.html)
- [The Birthday Paradox by Vsauce2 on Youtube](https://www.youtube.com/watch?v=ofTb57aZHZs)
- [Check your intuition: The birthday problem - David Knuffke on Youtube](https://www.youtube.com/watch?v=KtT_cgMzHx8)
