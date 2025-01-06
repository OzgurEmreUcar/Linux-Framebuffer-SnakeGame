# Linux Framebuffer Snake Game

Welcome to the Linux Framebuffer Snake Game! This project offers a fun way to explore graphics rendering in Linux by implementing the classic Snake game.

## Table of Contents

- [About the Project](#about-the-project)
- [Linux Framebuffer](#linux-framebuffer)
  - [What is the Framebuffer?](#what-is-the-framebuffer)
  - [Why Use the Framebuffer?](#why-use-the-framebuffer)
  - [Interacting with the Framebuffer](#interacting-with-the-framebuffer)
- [Getting Started](#getting-started)
  - [Prerequisites](#prerequisites)
  - [Installation](#installation)
- [Usage](#usage)
- [Important Notes](#important-notes)
- 
## About the Project

The Linux Framebuffer Snake Game is an educational project demonstrating how to use the Linux framebuffer for graphics rendering. It runs entirely in the terminal, bypassing the need for a GUI. The game showcases essential framebuffer operations, including drawing pixels, updating screens, and handling user input.

---

## Linux Framebuffer

### What is the Framebuffer?

The Linux framebuffer is a low-level graphics abstraction layer that allows applications to write directly to video memory. It provides a simple interface for drawing graphics on the screen without needing a windowing system like X11 or Wayland.

### Why Use the Framebuffer?

- **Lightweight**: Perfect for applications where minimal resource usage is critical.
- **Direct Control**: Gives direct access to pixel data, making it ideal for low-level graphics programming.
- **System Compatibility**: Works on a wide range of devices, from embedded systems to full desktop environments.

### Interacting with the Framebuffer

The framebuffer device is typically located at `/dev/fb0`. To interact with it:
1. **Open the framebuffer device** using system calls (e.g., `open("/dev/fb0", O_RDWR)` in C).
2. **Map video memory** to user space using `mmap`.
3. **Draw pixels** by directly modifying the framebuffer data.
4. **Refresh the screen** to display the updated content.


---

## Getting Started

To get a local copy up and running, follow these steps.

### Prerequisites

Ensure you have the following installed on your system:

- GCC (GNU Compiler Collection)
- Make
- Linux operating system with framebuffer support

### Installation

1. **Clone the repository**:
   ```bash
   git clone https://github.com/OzgurEmreUcar/Linux-Framebuffer-SnakeGame.git
2. **Navigate to the project directory**:
   ```bash
   cd Linux-Framebuffer-SnakeGame
3. **Build the project**:
   ```bash
   make
---

## Usage

1. **After building the project, run the game with the following command:**
   ```bash
   sudo ./snake_game
2. Controls:
  Arrow keys: Move the snake up, down, left, or right.
  Q: Quit the game.
---
### Important Notes:
 
  Superuser privileges are required to access the framebuffer device. This is why the sudo command is necessary.
  To run the game, you need direct access to the Linux framebuffer. This means disabling window management services such as GNOME, X11, or Wayland.
  The easiest way to do this is to switch to a virtual terminal (TTY) by pressing Ctrl + Alt + F2 (or F3, F4, etc.).
  Log in to the terminal, navigate to the project directory, and run the game.
  Ensure your system has a functioning framebuffer device, typically located at /dev/fb0. If this device is unavailable or inaccessible, the game will not run.
  To return to your graphical interface after playing, press Ctrl + Alt + F1 (or F7, depending on your system).
