# 🐚 MyShell — A Simple Custom Shell with a Python GUI

Hi there! 👋  
Welcome to MyShell, a lightweight Linux shell built from scratch in **C++**, wrapped in a clean Python Tkinter GUI. This project is a blend of system-level programming and user-friendly interface design — perfect for learners, hackers, and anyone curious about how shells work under the hood.

---

## 🚀 What MyShell Can Do

- 🖥️ Run basic Linux commands like `ls`, `cd`, `echo`, etc.
- 🧠 Keeps track of your command history
- 🎨 Comes with a Python GUI so you don't have to type in the terminal
- 🛠️ Built entirely from scratch — no frameworks or shortcuts
- 🌐 Works on Linux/WSL with optional Windows support

---

## 🛠️ How to Get Started

### Step 1: Compile the C++ Shell
Make sure you have `g++` and `readline` installed:
```bash
g++ myshell.cpp -o myshell -lreadline
