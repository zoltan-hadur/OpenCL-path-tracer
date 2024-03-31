#pragma once

#include <GL\glew.h>
#include <GL\glext.h>
#include <GL\freeglut.h>
#include <functional>
#include <vector>
#include <deque>
#include <map>
#include <string>
#include <ostream>
#include <sstream>
#include <algorithm>
#include "Stopwatch.h"
#include "ConsoleVariableManager.h"
#include "Color.h"



// Basic console for OpenGL applications written in C++
// Class is based on https://github.com/arpg/CVars
class GLConsole
{
private:
    PFNGLWINDOWPOS2IPROC glWindowPos2i;			// Function that needs to be loaded dynamically. It's sets the raster position
    enum State
    {								// States for drawing the console window
        CLOSED, ROLLING_DOWN, INTERFACE_APPEARING, OPENED, INTERFACE_DISAPPEARING, ROLLING_UP
    };
    State state;								// The state of the console (opened, closed, etc)
    Stopwatch watch_render;						// To get elapsed time since last call (i.e. dt)
    Stopwatch watch_command;					// For command wait

    static std::map < std::string, std::function<void(std::vector<std::string>) >> funcs;	// Function pointers

    float animl_rolling;						// Time needed to roll down/up the console window in seconds
    float animl_interface;						// Time needed for the console interface to appear/disappear in seconds
    float animl_cursor;							// Frequency of the cursor's blinking in Hz
    float acc_rolling;							// Controls the animation
    float acc_interface;						// Controls the animation
    float acc_cursor;							// Controls the animation
    float overlap;								// How much percent the console window takes from the screen
    int scroll_lines;							// Number of lines scrolled when the mouse wheels move
    Color color_background;					// Color of the console window
    Color color_interface;						// Color of the console's interface
    Color color_text;							// Color of the text
    Color color_text_selection;				// Color of the selected text
    float pos_scrollbar;						// Position of the scrollbar
    int pos_scroll;								// Determines how much lines were shifted in the output buffer
    int size_buffer_output;						// The output buffer's size
    std::deque<std::string> buffer_output;		// Stores everything what was printed to the console
    int pos_buffer_command;						// Index pointing to a command in the command buffer
    int size_buffer_command;					// Command_history's size
    std::deque<std::string> buffer_command;		// List with all the commands that the user previously entered
    std::deque<std::string> buffer_commands;	// List of commands waiting for execution
    std::deque<std::string> buffer_commands_single;
    int pos_cursor;								// Position of the cursor to manipulate the input buffer
    int pos_selection;							// Position of where the cursor was at when the selection started
    bool selecting_buffer_input;				// True if user currently selecting characters from the input buffer
    bool pressed_shift;							// True if shift is pressed
    std::deque<unsigned char> buffer_input;		// Stores the characters that the user writes to the console. Also can be manipulated like in nearly every text editor


    void animl_rolling_changed();				// Handle when animl_rolling changes
    void animl_interface_changed();				// Handle when animl_interface changes
    void roll_console(float dt);				// Draws the console window when it is opening (rolling down) or closing (rolling up)
    void draw_console(float dt);				// Draws the console window, interface and the text
    void draw_selection(int x, int y);			// Draws quads upon selected text in the buffer_input
    std::deque<std::string> handle_overflow(std::deque<std::string>& lines_original, int max_lines);	// Returns with a list of lines (strings) that fits into the console window
    std::deque<std::string> handle_overflow(std::deque<unsigned char>& line_original, int max_lines);	// Returns with a list of lines (strings) that fits into the console window
    void delete_char_before();					// Deletes the char before the cursor from the input buffer (like pressing backspace in a text editor)
    void delete_char_after();					// Deletes the char after the cursor from the input buffer (like pressing delete in a text editor)
    void insert_char(unsigned char key);		// Inserts a character at the cursor's position
    void insert_paste(std::string paste);		// Inserts a whole string at the cursor's position
    std::string get_clipboard_text();			// Returns with the string stored in the global clipboard (ctrl+v)
    void set_clipboard_text(std::string copy);	// Sets the global clipboard with the string selected in the input buffer (ctrl+c, ctrl+x)
    void process_cout();						// Handle when something writes to the console using GLConsole::cout
    void buffer_command_previous();				// Sets the input buffer to the previous command (like pressing the up arrow in Command Prompt)
    void buffer_command_next();					// Sets the input buffer to the next command (like pressing the down arrow in Command Prompt)
    void cursor_move_left();					// Moves the cursor to the left by one (like pressing left arrow in a text editor)
    void cursor_move_right();					// Moves the cursor to the right by one (like pressing the right arrow in a text editor)
    void cursor_jump_left();					// Moves the cursor to the top left (like pressing home in a text editor)
    void cursor_jump_right();					// Moves the cursor to the top right (like pressing end in a text editor)
    void complete_command();					// Completes the partially writed command in the input buffer (like pressing tab in Command Prompt)
    void process_commands();					// Processes the commands in the commands buffer
    void process_command(std::string command);	// Processes a single command
public:
    static ConsoleVariableManager cvars;					// Variables that are attached to normal variables, and vice versa modifiable from the console
    static std::ostringstream cout;				// Console out, works like std::cout

    GLConsole();								// Does nothing. Must call init to work properly
    void init();								// Initializing the console. Should only get called once, should only have one GLConsole instance and must be called after glutInit was called
    void reset();								// Resets the console to the default state
    void open();								// Opens the console window
    void close();								// Closes the console window
    void toggle();								// Opens the console window if it was closed, and vice versa
    bool is_open();								// Returns true if console is open and usable
    void draw();								// Draws the whole console (animations, window, interface, text, everything), also handles the cout
    void process();								// Process commands and cout
    void on_keyboard(unsigned char key);		// Keyboard event handler
    void on_special(int key);					// Keyboard event handler for special keys (arrow, f1,2,3..., etc)
    void shift_pressed();						// Sets pressed_shift to true
    void shift_released();						// Sets pressed_shift to false
    void scroll_up();							// Scrolls up in the output text
    void scroll_down();							// Scroll down in the output text
    void print_help();							// Prints help
    static void add_function(std::string name, std::function<void(std::vector<std::string>)> f);	// Store the function pointer
    static void remove_function(std::string name);								// Removes the function pointer
};