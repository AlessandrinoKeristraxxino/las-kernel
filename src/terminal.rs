// crate/src/terminal.rs

use crate::ffi::{vga_write, vga_set_color, vga_clear, vga_putchar};
use alloc::format;
use alloc::string::String;
use alloc::vec::Vec;

pub static TERMINAL: Mutex<Option<Terminal>> = Mutex::new(None);

#[repr(u8)]
#[allow(dead_code)]
pub enum Color {
    Black = 0x0,
    Blue = 0x1,
    Green = 0x2,
    Cyan = 0x3,
    Red = 0x4,
    Magenta = 0x5,
    Brown = 0x6,
    LightGray = 0x7,
    DarkGray = 0x8,
    LightBlue = 0x9,
    LightGreen = 0xA,
    LightCyan = 0xB,
    LightRed = 0xC,
    LightMagenta = 0xD,
    Yellow = 0xE,
    White = 0xF,
}

pub struct Terminal {
    buffer: String,
    prompt: &'static str,
    history: Vec<String>,
}

impl Terminal {
    pub fn new() -> Self {
        Terminal {
            buffer: String::new(),
            prompt: "AlessandroNapoli@las-os >> \0",
            history: Vec::new(),
        }
    }

    pub fn init(&mut self) {
        unsafe {
            vga_clear();
        }
        self.print_prompt();
    }

    pub fn handle_key(&mut self, key: char) {
        match key {
            '\n' => {
                unsafe {
                    vga_write(b"\n\0".as_ptr() as *const _);
                }

                self.execute_command();
                self.buffer.clear();

                self.print_prompt();
            },
            '\x08' => {
                // Backspace
                if !self.buffer.is_empty() {
                    self.buffer.pop();
                    
                    unsafe {
                        vga_putchar(b'\x08');
                        vga_putchar(b' ');
                        vga_putchar(b'\x08');
                    }
                }
            },
            c => {
                self.buffer.push(c);
                unsafe {
                    vga_putchar(c as u8);
                }
            }
        }
    }

    fn execute_command(&mut self) {
        let input = self.buffer.trim();
        if input.is_empty() {
            return;
        }

        self.history.push(String::from(input));
        match input {
            "help" => {
                self.help();
            },
            "clear" => {
                unsafe {
                    vga_clear();
                }
            },
            "about" => {
                self.about();
            },
            cmd => {
                let error_msg = format!("Command not found: {}\n\0", cmd);
                unsafe {
                    vga_write(error_msg.as_ptr() as *const _);
                }
            }
        }
    }

    fn help(&self) {
        unsafe {
            vga_write("Available commands:\n\0".as_ptr() as *const _);
            vga_write("  help  - Show this help message\n\0".as_ptr() as *const _);
            vga_write("  clear - Clear the screen\n\0".as_ptr() as *const _);
            vga_write("  about - Show OS information\n\0".as_ptr() as *const _);
        }
    }

    fn about(&self) {
        unsafe {
            vga_write("las-os Kernel v0.1.0\n\0".as_ptr() as *const _);
            vga_write("Written in Rust & C\n\0".as_ptr() as *const _);
        }
    }

    fn print_prompt(&self) {
        unsafe {
            vga_set_color(Color::LightGreen as u8, Color::Black as u8);
            vga_write(self.prompt.as_ptr() as *const _);

            vga_set_color(Color::White as u8, Color::Black as u8);
        }    
    }
}

/// Global terminal initialization
pub fn init_global_terminal() {
    let mut term = Terminal::new();
    term.init();
    *TERMINAL.lock() = Some(term);
}

