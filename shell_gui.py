import tkinter as tk
from tkinter import scrolledtext
import subprocess
import os

class ShellGUI:
    def __init__(self, root):
        self.root = root
        self.root.title("MyShell GUI")

        self.output_area = scrolledtext.ScrolledText(root, width=80, height=20, bg="black", fg="lime", insertbackground="white")
        self.output_area.pack(padx=10, pady=10)
        self.output_area.configure(state='disabled')

        self.command_entry = tk.Entry(root, width=80, bg="white")
        self.command_entry.pack(padx=10, pady=(0,10))
        self.command_entry.bind("<Return>", self.run_command)

        self.command_entry.focus()

    def run_command(self, event=None):
        cmd = self.command_entry.get().strip()
        if not cmd:
            return

        # Clear command entry
        self.command_entry.delete(0, tk.END)

        # Run C++ shell binary and pass the input
        try:
            result = subprocess.run(
                ['./myshell'],
                input=cmd,
                text=True,
                capture_output=True,
                timeout=5
            )

            output = result.stdout + result.stderr
        except Exception as e:
            output = f"[Error] {e}"

        # Display output in the text area
        self.output_area.configure(state='normal')
        self.output_area.insert(tk.END, f"> {cmd}\n{output}\n")
        self.output_area.configure(state='disabled')
        self.output_area.see(tk.END)


if __name__ == "__main__":
    root = tk.Tk()
    app = ShellGUI(root)
    root.mainloop()
