// crate/src/terminal.rs

use crate::ffi::{vga_write, vga_set_color, vga_clear, vga_putchar};
use alloc::string::String;
use alloc::vec::Vec;

#[repr(u8)]
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
}

impl Terminal {
    pub fn new() -> Self {
        Terminal {
            buffer: String::new(),
            prompt: "AlessandroNapoli@las-os >> ",
        }
    }

    pub fn init(&mut self) {
        self.print_prompt();
    }

    pub fn handle_key(&mut self, key: char) {
        match key {
            '\n' => {
                self.execute_command();
                self.buffer.clear();
                self.print_prompt();
            },
            '\x08' => {
                if !self.buffer.is_empty() {
                    self.buffer.pop();
                }
            },
            c => {
                self.buffer.push(c);
            }
        }
    }

    fn execute_command(&mut self) {
        let input = self.buffer.trim();
        if input.is_empty() {
            return;
        }

        match input {
            "help" =>   {
                //show the commands list
            },
            "clear" => {
                //clear command
            }
            "about" => {
                //abpit command
            },
            cmd => {
                // unknown command
            }
        }
    }

    fn print_prompt(&self) {
        unsafe {
            
        }    
    }
}