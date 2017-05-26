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
#include "CVarContainer.h"
#include "float3.h"



// Basic console for OpenGL applications written in C++
// Class is based on https://github.com/arpg/CVars
class GLConsole {
private:
	PFNGLWINDOWPOS2IPROC glWindowPos2i;			// Function that needs to be loaded dynamically. It's sets the raster position
	enum State {								// States for drawing the console window
		CLOSED, ROLLING_DOWN, INTERFACE_APPEARING, OPENED, INTERFACE_DISAPPEARING, ROLLING_UP
	};
	State state;								// The state of the console (opened, closed, etc)
	Stopwatch watch_render;						// To get elapsed time since last call (i.e. dt)
	Stopwatch watch_command;					// For command wait

	static std::map < std::string, std::function<void(void)>> funcs;	// Function pointers

	float animl_rolling;						// Time needed to roll down/up the console window in seconds
	float animl_interface;						// Time needed for the console interface to appear/disappear in seconds
	float animl_cursor;							// Frequency of the cursor's blinking in Hz
	float acc_rolling;							// Controls the animation
	float acc_interface;						// Controls the animation
	float acc_cursor;							// Controls the animation
	float overlap;								// How much percent the console window takes from the screen
	int scroll_lines;							// Number of lines scrolled when the mouse wheels move
	float3 color_background;					// Color of the console window
	float color_background_transparency;		// Transparency of the console window
	float3 color_interface;						// Color of the console's interface
	float3 color_text;							// Color of the text
	float3 color_text_selection;				// Color of the selected text
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
	static CVarContainer cvars;					// Variables that are attached to normal variables, and vice versa modifiable from the console
	static std::ostringstream cout;				// Console out, works like std::cout

	GLConsole();								// Does nothing. Must call init to work properly
	void init();								// Initializing the console. Should only get called once, should only have one GLConsole instance and must be called after glutInit was called
	void reset();								// Resets the console to the default state
	void open();								// Opens the console window
	void close();								// Closes the console window
	void toggle();								// Opens the console window if it was closed, and vice versa
	bool is_open();								// Returns true if console is open and usable
	void draw();								// Draws the whole console (animations, window, interface, text, everything), also handles the cout
	void on_keyboard(unsigned char key);		// Keyboard event handler
	void on_special(int key);					// Keyboard event handler for special keys (arrow, f1,2,3..., etc)
	void shift_pressed();						// Sets pressed_shift to true
	void shift_released();						// Sets pressed_shift to false
	void scroll_up();							// Scrolls up in the output text
	void scroll_down();							// Scroll down in the output text
	void print_help();							// Prints help
	static void add_function(std::string name, std::function<void(void)> f);	// Store the function pointer
	static void remove_function(std::string name);								// Removes the function pointer
};

std::map<std::string, std::function<void(void)>> GLConsole::funcs;	// Static variables need to be defined
CVarContainer GLConsole::cvars;										// Static variables need to be defined
std::ostringstream GLConsole::cout;									// Static variables need to be defined

GLConsole::GLConsole() {
	// Does nothing
}

void GLConsole::init() {
	glWindowPos2i = (PFNGLWINDOWPOS2IPROC)glutGetProcAddress("glWindowPos2i");	// Loads the function

	state = CLOSED;
	watch_render.start();
	watch_command.start();

	animl_rolling = 0.33f;						cvars.attach_cvar<float>("console.animations.rolling", &animl_rolling, "Time needed to roll down/up the console window in seconds. Interval: [0, infty).", std::bind(&GLConsole::animl_rolling_changed, this));
	animl_interface = 0.25f;					cvars.attach_cvar<float>("console.animations.interface", &animl_interface, "Time needed for the console interface to appear/disappear in seconds. Interval: (0, infty).", std::bind(&GLConsole::animl_interface_changed, this));
	animl_cursor = 1.0f;						cvars.attach_cvar<float>("console.animations.cursor", &animl_cursor, "Frequency of the cursor's blinking in Hz. Interval: [0.1, infty).");
	acc_rolling = 0.0f;
	acc_interface = 0.0f;
	acc_cursor = 0.0f;
	overlap = 0.68f;							cvars.attach_cvar<float>("console.overlap", &overlap, "How much percent the console window takes from the screen. Interval: [0.2, 1].");
	scroll_lines = 1;							cvars.attach_cvar<int>("console.buffers.scroll_lines", &scroll_lines, "Number of lines scrolled. Interval: [1, infty).");
	color_background = float3(0, 0.5f, 1);		cvars.attach_cvar<float3>("console.colors.background", &color_background, "Color of the console window in RGB. Interval: [0, 1].");
	color_background_transparency = 0.68f;		cvars.attach_cvar<float>("console.colors.background_transparency", &color_background_transparency, "Transparency of the console window, i.e. Alpha channel. Interval: [0, 1].");
	color_interface = float3(1, 1, 1);			cvars.attach_cvar<float3>("console.colors.interface", &color_interface, "Color of the console interface in RGB. Interval: [0, 1].");
	color_text = float3(1, 1, 1);				cvars.attach_cvar<float3>("console.colors.text", &color_text, "Color of the text in RGB. Interval: [0, 1].");
	color_text_selection = float3(1, 0, 0);		cvars.attach_cvar<float3>("console.colors.text_selection", &color_text_selection, "Color of the selected text in RGB. Interval: [0, 1].");
	pos_scrollbar = 0.0f;
	pos_scroll = 0;
	size_buffer_output = 1000;					cvars.attach_cvar<int>("console.buffers.output", &size_buffer_output, "The output buffer's size. Interval: [100, infty).");
	buffer_output = std::deque<std::string>();
	pos_buffer_command = -1;
	size_buffer_command = 50;					cvars.attach_cvar<int>("console.buffers.command", &size_buffer_command, "The command buffer's size. Interval: [10, infty).");
	buffer_command = std::deque<std::string>();
	pos_cursor = 0;
	pos_selection = 0;
	selecting_buffer_input = false;
	pressed_shift = false;
	buffer_input = std::deque<unsigned char>();
}

void GLConsole::reset() {
	animl_rolling = 0.33f;
	animl_interface = 0.25f;
	animl_cursor = 1.0f;
	overlap = 0.68f;
	scroll_lines = 1;
	color_background = float3(0, 0.5f, 1);
	color_background_transparency = 0.68f;
	color_interface = float3(1, 1, 1);
	color_text = float3(1, 1, 1);
	color_text_selection = float3(1, 0, 0);
	size_buffer_output = 1000;
	pos_buffer_command = -1;
	size_buffer_command = 50;
	pos_cursor = 0;
	pos_selection = 0;
	selecting_buffer_input = false;
	pressed_shift = false;
	buffer_input.clear();
}

void GLConsole::open() {
	if (state == CLOSED) {					// If the console is closed,
		state = ROLLING_DOWN;				// starts a ROLLING_DOWN->INTERFACE_APPEARING->OPENED process
	}
}

void GLConsole::close() {
	if (state == OPENED) {					// If the console is opened,
		state = INTERFACE_DISAPPEARING;		// starts an INTERFACE_DISAPPEARING->ROLLING_UP->CLOSED process
	}
}

void GLConsole::toggle() {
	if (state == OPENED) {					// If the console is opened,
		this->close();						// closes it
	} else if (state == CLOSED) {			// Else if the console is closed,
		this->open();						// opens it
	}
}

bool GLConsole::is_open() {
	return state == OPENED;					// Returns true if the console is opened
}

void GLConsole::draw() {
	glEnable(GL_BLEND); glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glBindTexture(GL_TEXTURE_2D, 0);
	float dt = watch_render.get_delta_time();		// Gets the elapsed time in seconds since the last call
	this->process_commands();				// Execute commands from the command list
	this->process_cout();					// Handle if something printed to the console
	if (state == ROLLING_DOWN || state == ROLLING_UP) {
		this->roll_console(dt);				// Animate opening/closing
	} else if (state == OPENED || state == INTERFACE_APPEARING || state == INTERFACE_DISAPPEARING) {
		this->draw_console(dt);				// Animate interface appearing/disapperaing and drawing text
	}
}

void GLConsole::on_keyboard(unsigned char key) {
	switch (key) {
		case 3:		// Ctrl + c
			if (selecting_buffer_input) {																			// If selecting text
				int from = std::min(pos_selection, pos_cursor);														// Selection starts at this index
				int to = std::max(pos_selection, pos_cursor);														// Selection ends at this index
				std::string selected_text = std::string(buffer_input.begin() + from, buffer_input.begin() + to);	// The selected text in the input buffer
				this->set_clipboard_text(selected_text);															// Copy it to the clipboard
			}
			break;
		case 22:	// Ctrl + v
			this->insert_paste(this->get_clipboard_text());															// Pastes string from clipboard to the input buffer
			break;
		case 24:	// Ctrl + x
			if (selecting_buffer_input) {
				int from = std::min(pos_selection, pos_cursor);														// Selection starts at this index
				int to = std::max(pos_selection, pos_cursor);														// Selection ends at this index
				std::string selected_text = std::string(buffer_input.begin() + from, buffer_input.begin() + to);	// The selected text in the input buffer
				this->set_clipboard_text(selected_text);															// Copy it to the clipboard
				this->delete_char_before();																			// And remove it from the input buffer
			}
			break;
		case 9:		// Tab key
			this->complete_command();																				// Completes a CVar's name
			break;
		case 8:		// Backspace key
			this->delete_char_before();																				// Acts like backspace in every text editor
			break;
		case 13:	// Enter key
			buffer_commands.push_back(std::string(buffer_input.begin(), buffer_input.end()));						// Add the command from the input buffer to the commands buffer
			buffer_input.clear();																					// Clear input buffer
			this->cursor_jump_left();																				// And set the cursor to the top left

		break;
		case 127:	// Delete key
			this->delete_char_after();																				// Acts like delete in every text editor
			break;
		default:	// Insert char at cursor
			this->shift_released();	// At insertion for example 'A' shift is pressed, but not currently selecting text, hence this function call
			this->insert_char(key);
	}
}

void GLConsole::on_special(int key) {
	switch (key) {
		case GLUT_KEY_UP:
			this->buffer_command_previous();
			break;
		case GLUT_KEY_DOWN:
			this->buffer_command_next();
			break;
		case GLUT_KEY_LEFT:
			this->cursor_move_left();
			break;
		case GLUT_KEY_RIGHT:
			this->cursor_move_right();
			break;
		case GLUT_KEY_HOME:
			this->cursor_jump_left();
			break;
		case GLUT_KEY_END:
			this->cursor_jump_right();
			break;
		case GLUT_KEY_PAGE_UP:
			this->scroll_up();
			break;
		case GLUT_KEY_PAGE_DOWN:
			this->scroll_down();
			break;
	}
}

void GLConsole::shift_pressed() {
	pressed_shift = true;
}

void GLConsole::shift_released() {
	pressed_shift = false;
}

void GLConsole::scroll_up() {
	int width = glutGet(GLUT_WINDOW_WIDTH); int height = glutGet(GLUT_WINDOW_HEIGHT);	// Current size of window for handle too long lines
	int char_width = 8; int char_height = 14;											// Used font's size
	int max_output_lines = overlap * height / char_height - 1;							// Maximum number of lines that fits into the console

	int max_scroll_pos = this->handle_overflow(buffer_output, size_buffer_output).size() - max_output_lines;	// To don't "scroll out"
	if (pos_scroll < max_scroll_pos) {
		int new_scroll_position = std::min(max_scroll_pos, pos_scroll + scroll_lines);
		for (int i = 0; i < new_scroll_position - pos_scroll; ++i) {					// Scrolling is in fact is circshifting the output buffer
			buffer_output.push_back(buffer_output.front());
			buffer_output.pop_front();
		}
		pos_scroll = new_scroll_position;
	}
	pos_scrollbar = (float)pos_scroll / max_scroll_pos;									// Sets the scrollbar position
}

void GLConsole::scroll_down() {
	int width = glutGet(GLUT_WINDOW_WIDTH); int height = glutGet(GLUT_WINDOW_HEIGHT);	// Current size of window for handle too long lines
	int char_width = 8; int char_height = 14;											// Used font's size
	int max_output_lines = overlap * height / char_height - 1;							// Maximum number of lines that fits into the console

	int max_scroll_pos = this->handle_overflow(buffer_output, size_buffer_output).size() - max_output_lines;	// To don't "scroll out"
	if (pos_scroll > 0) {
		int new_scroll_position = std::max(0, pos_scroll - scroll_lines);
		for (int i = 0; i < pos_scroll - new_scroll_position; ++i) {					// Scrolling is in fact is circshifting the output buffer
			buffer_output.push_front(buffer_output.back());
			buffer_output.pop_back();
		}
		pos_scroll = new_scroll_position;
	}
	pos_scrollbar = (float)pos_scroll / max_scroll_pos;									// Sets the scrollbar position
}

void GLConsole::print_help() {
	cout << "----------------------------------HELP----------------------------------\n";
	cout << "Functions:\n";
	for (auto f : funcs) {
		cout << " " << f.first << "\n";
	}
	cout << "Commands:\n";
	cout << " Help: prints this help message\n";
	cout << " List: lists all console variables\n";
	cout << " Cls: clears the screen\n";
	cout << " Start [name]: starts the script named [name]\n";
	cout << " Wait [x]: waits x seconds until next command execution\n";
	cout << " Reset: resets the console to the default state\n";
	cout << " Exit: completly exits the application\n";
	cout << "Other:\n";
	cout << " Up arrow: previous command\n";
	cout << " Down arrow: next command\n";
	cout << " Tab: auto completes command\n";
	cout << " [command]: prints the variable's name, value and it's description\n";
	cout << " [command] = [value]: sets the [command] variable's value to [value]\n";
	cout << "Notes:\n";
	cout << " Commands must end with a semicolon to execute them\n";
	cout << " You can write multiple commands separated by semicolons\n";
	cout << "----------------------------------HELP----------------------------------\n";
}

void GLConsole::add_function(std::string name, std::function<void(void)> f) {
	funcs.insert({ name, f });
}

void GLConsole::remove_function(std::string name) {
	funcs.erase(name);
}


void GLConsole::animl_rolling_changed() {
	acc_rolling = animl_rolling;
}

void GLConsole::animl_interface_changed() {
	acc_interface = animl_interface;
}

void GLConsole::roll_console(float dt) {
	if (state == ROLLING_DOWN) {									// If the console window is rolling down
		acc_rolling = std::min(acc_rolling + dt, animl_rolling);	// Increment the variable
		if (acc_rolling == animl_rolling) {							// When the animation finishes
			state = INTERFACE_APPEARING;							// Move to the next state
		}
	} else if (state == ROLLING_UP) {								// If the console window is rolling up
		acc_rolling = std::max(acc_rolling - dt, 0.0f);				// Decrement the variable
		if (acc_rolling == 0.0f) {									// When the animation finishes
			state = CLOSED;											// Move to the next state
		}
	}
	acc_rolling = tanh(acc_rolling / animl_rolling * 2) / tanh(2) * animl_rolling;		// Non linear scale for better animation effect
	// Draw the console's window according to the elapsed time since the start of the animation
	glColor4f(color_background[0], color_background[1], color_background[2], color_background_transparency);
	glBegin(GL_QUADS);
	glVertex2f(-1, 1);
	glVertex2f(1, 1);
	glVertex2f(1, 1 - 2 * (overlap * acc_rolling / animl_rolling));	// acc_rolling == 0 -> the console window is not visible
	glVertex2f(-1, 1 - 2 * (overlap * acc_rolling / animl_rolling));// acc_rolling == animl_rolling -> the console window is fully visible
	glEnd();
	acc_rolling = atanh(acc_rolling / animl_rolling * tanh(2)) / 2 * animl_rolling;		// Scaling back to linear
}

void GLConsole::draw_console(float dt) {
	// Variables
	int width = glutGet(GLUT_WINDOW_WIDTH); int height = glutGet(GLUT_WINDOW_HEIGHT);	// Current size of window for handle too long lines
	int console_x = 2; int console_y = (1.0f - overlap) * height + 4;					// Bottom left corner of console
	int char_width = 8; int char_height = 14;											// Used font's size
	int max_chars_in_line = (width - 2 * char_width) / char_width;						// Maximum number of characters per line that fits in to the window
	int max_output_lines = overlap * height / char_height - 1;							// Maximum number of lines that fits in to the console

	// Draw the background
	glColor4f(color_background[0], color_background[1], color_background[2], color_background_transparency);	// Set background color
	glBegin(GL_QUADS);
	glVertex2f(-1, 1);
	glVertex2f(1, 1);
	glVertex2f(1, 1 - 2 * overlap);
	glVertex2f(-1, 1 - 2 * overlap);
	glEnd();

	// Draw the input line(s)
	// Interface/text transparency depends on acc_interface
	// acc_interface == 0 -> not visible
	// acc_interface == animl_interface -> visible
	acc_interface = pow(acc_interface / animl_interface, 2);								// Human eye senses brightness not linearly
	auto input_lines = this->handle_overflow(buffer_input, max_output_lines);				// Get lines from input buffer
	int input_overflow_shift = (input_lines.size() - 1) * char_height;						// Determines the value needed to translate the raster position in pixels
	glColor4f(color_interface[0], color_interface[1], color_interface[2], acc_interface);	// Sets color
	glWindowPos2i(console_x, console_y + input_overflow_shift);								// If no overflow, the cursor is at the bottom left side of the console by default
	glutBitmapCharacter(GLUT_BITMAP_8_BY_13, '>');											// The cursor is part of the interface
	for (int i = 0; i < input_lines.size(); ++i) {											// Start drawing the text in the input buffer
		glWindowPos2i(console_x + char_width, console_y + i * char_height);
		for (int j = 0; j < input_lines[i].size(); ++j) {									// It happens char by char to handle selection
			unsigned char c = input_lines[i][j];
			int b_pos = (input_lines.size() - 1 - i) * max_chars_in_line + j;				// Calculate a theoretical cursor position to know if the currently drawed char is selected
			int rasterpos[4];
			glGetIntegerv(GL_CURRENT_RASTER_POSITION, rasterpos);
			if (std::min(pos_selection, pos_cursor) <= b_pos && b_pos < std::max(pos_selection, pos_cursor) && selecting_buffer_input) {	// Check if char is in the selection
				this->draw_selection(rasterpos[0], rasterpos[1]);
			}
			glColor4f(color_text[0], color_text[1], color_text[2], acc_interface);
			glWindowPos2i(rasterpos[0], rasterpos[1]);
			glutBitmapCharacter(GLUT_BITMAP_8_BY_13, c);
		}
	}

	// Draw the cursor
	// The cursor is visible only in the first half
	glColor4f(color_interface[0], color_interface[1], color_interface[2], acc_interface);
	acc_cursor = fmod(acc_cursor + dt, animl_cursor);
	if (acc_cursor < animl_cursor / 2) {
		int x_pos = console_x + (pos_cursor % max_chars_in_line + 1) * char_width;
		int y_pos = console_y - 2 + ((input_lines.size() - 1) - pos_cursor / max_chars_in_line) * char_height;
		glWindowPos2i(x_pos, y_pos);
		glutBitmapCharacter(GLUT_BITMAP_8_BY_13, '_');
	}

	// Draw buffer_output
	glColor4f(color_text[0], color_text[1], color_text[2], acc_interface);
	auto output_lines = this->handle_overflow(buffer_output, max_output_lines);	// Get only enough lines to fill the console window from output buffer
	for (int i = 0; i < output_lines.size(); ++i) {								// Print every line
		glWindowPos2i(console_x + char_width, console_y + input_overflow_shift + (i + 1) * char_height);
		glutBitmapString(GLUT_BITMAP_8_BY_13, (const unsigned char*)output_lines[i].c_str());
	}

	// Draw scrollbar and input line box
	glMatrixMode(GL_PROJECTION); glPushMatrix(); glLoadIdentity();							// Save current projection matrix
	glOrtho(0.0, glutGet(GLUT_WINDOW_WIDTH), 0.0, glutGet(GLUT_WINDOW_HEIGHT), -1.0, 1.0);	// Transform it to able to draw in pixel coordinates
	glMatrixMode(GL_MODELVIEW); glPushMatrix(); glLoadIdentity();							// Save current modelview matrix
	glBegin(GL_QUADS);
	// Draw scrollbar region
	int start_x = width - 5; int start_y = height * (1 - overlap) + input_overflow_shift + char_height + 2;
	glColor4f(color_interface[0], color_interface[1], color_interface[2], acc_interface);
	glVertex2f(start_x, start_y);							// Bottom left
	glVertex2f(start_x, height);							// Top left
	glVertex2f(start_x + 5, height);						// Top right
	glVertex2f(start_x + 5, start_y);						// Bottom right
	// Draw scrollbar slider
	int shift = (height - (start_y + 20)) * pos_scrollbar;	// Shift for the slider
	glColor4f(color_interface[0] * 0.7f, color_interface[1] * 0.7f, color_interface[2] * 0.7f, acc_interface);	// It's color is a little darker to be visible
	glVertex2f(start_x, start_y + shift);					// Bottom left
	glVertex2f(start_x, start_y + 20 + shift);				// Top left
	glVertex2f(start_x + 5, start_y + 20 + shift);			// Top right
	glVertex2f(start_x + 5, start_y + shift);				// Bottom right
	glEnd();
	glBegin(GL_LINE_LOOP);
	// Draw input line box
	start_x = 0; start_y = height * (1 - overlap);
	glColor4f(color_interface[0], color_interface[1], color_interface[2], acc_interface);
	glVertex2f(start_x, start_y);														// Bottom left
	glVertex2f(start_x, start_y + input_lines.size() * char_height + 2);				// Top left
	glVertex2f(start_x + width - 1, start_y + input_lines.size() * char_height + 2);	// Top right
	glVertex2f(start_x + width - 1, start_y);											// Bottom right
	glEnd();
	glPopMatrix(); glMatrixMode(GL_PROJECTION); glPopMatrix(); glMatrixMode(GL_MODELVIEW);	// Load the previously saved matrices

	acc_interface = sqrt(acc_interface) * animl_interface;	// Scaling back
	if (state == INTERFACE_APPEARING) {
		acc_interface = std::min(acc_interface + dt, animl_interface);
		if (acc_interface == animl_interface) {
			state = OPENED;
		}
	} else if (state == INTERFACE_DISAPPEARING) {
		acc_interface = std::max(acc_interface - dt, 0.0f);
		if (acc_interface == 0.0f) {
			state = ROLLING_UP;
		}
	}
}

void GLConsole::draw_selection(int x, int y) {
	int char_width = 8; int char_height = 15; y = y - 3;

	glMatrixMode(GL_PROJECTION); glPushMatrix(); glLoadIdentity();							// Save current projection matrix
	glOrtho(0.0, glutGet(GLUT_WINDOW_WIDTH), 0.0, glutGet(GLUT_WINDOW_HEIGHT), -1.0, 1.0);	// Transform it to able to draw in pixel coordinates
	glMatrixMode(GL_MODELVIEW); glPushMatrix(); glLoadIdentity();							// Save current modelview matrix
	glColor3f(color_text_selection[0], color_text_selection[1], color_text_selection[2]);
	glBegin(GL_QUADS);
	glVertex2f(x, y);
	glVertex2f(x, y + char_height);
	glVertex2f(x + char_width, y + char_height);
	glVertex2f(x + char_width, y);
	glEnd();
	glPopMatrix(); glMatrixMode(GL_PROJECTION);
	glPopMatrix(); glMatrixMode(GL_MODELVIEW);
}

std::deque<std::string> GLConsole::handle_overflow(std::deque<std::string>& lines_original, int max_lines) {
	int width = glutGet(GLUT_WINDOW_WIDTH); int height = glutGet(GLUT_WINDOW_HEIGHT);	// Current size of window for handle too long lines
	int char_width = 8; int char_height = 14;											// Used font's size
	int max_chars_in_line = (width - 2 * char_width) / char_width;						// Maximum number of characters per line that fits in to the window

	std::deque<std::string> lines;
	for (auto line_original : lines_original) {											// Iterating through original lines of text
		std::deque<std::string> lines_overflowed;
		while (line_original.size() > max_chars_in_line) {								// Some lines are too long, so we divide them into multiple line
			std::string line_overflowed = line_original.substr(0, max_chars_in_line);	// Get string that as wide as the screen, so it can fit in it
			lines_overflowed.push_front(line_overflowed);								// Collect these overflowed lines into a separate buffer
			line_original = line_original.substr(line_overflowed.size());				// Remove the string that we got above
		}
		lines_overflowed.push_front(line_original);										// Push the last overflowed string into the buffer
		for (auto line_overflowed : lines_overflowed) {									// Iterate through them, and add them to the new buffer
			lines.push_back(line_overflowed);
			if (lines.size() >= max_lines) {											// But only max_lines line, becouse of finite resources
				return lines;
			}
		}
	}
	return lines;
}

std::deque<std::string> GLConsole::handle_overflow(std::deque<unsigned char>& line_original, int max_lines) {
	return this->handle_overflow(std::deque<std::string>({ std::string(buffer_input.begin(), buffer_input.end()) }), max_lines);
}

void GLConsole::delete_char_before() {
	if (selecting_buffer_input) {												// Delete the whole selection when selecting_buffer_input
		buffer_input.erase(buffer_input.begin() + std::min(pos_selection, pos_cursor), buffer_input.begin() + std::max(pos_selection, pos_cursor));
		this->cursor_move_left();												// Then move the cursor to the top right of the selection
	} else if (buffer_input.size() > 0 && pos_cursor > 0) {						// Else delete the character right before the cursor
		buffer_input.erase(buffer_input.begin() + (--pos_cursor));
	}
}

void GLConsole::delete_char_after() {
	if (selecting_buffer_input) {												// Delete the whole selection when selecting_buffer_input
		buffer_input.erase(buffer_input.begin() + std::min(pos_selection, pos_cursor), buffer_input.begin() + std::max(pos_selection, pos_cursor));
		this->cursor_move_left();												// Then move the cursor to the top left of the selection
	} else if (buffer_input.size() > 0 && pos_cursor < buffer_input.size()) {	// Else delete the character right after the cursor
		buffer_input.erase(buffer_input.begin() + pos_cursor);
	}
}

void GLConsole::insert_char(unsigned char key) {
	if (selecting_buffer_input) {												// WHen selecting_buffer_input
		this->delete_char_before();												// Delete the whole selection
	}
	buffer_input.insert(buffer_input.begin() + (pos_cursor++), key);			// Then insert the char into the input buffer
	selecting_buffer_input = false;
}

void GLConsole::insert_paste(std::string paste) {
	if (selecting_buffer_input) {												// When selecting_buffer_input
		this->delete_char_before();												// Delete the whole selection
	}
	for (unsigned char c : this->get_clipboard_text()) {						// Then insert the string into the input line
		this->insert_char(c);
	}
	selecting_buffer_input = false;
}

std::string GLConsole::get_clipboard_text() {
	std::string str = "";
	if (IsClipboardFormatAvailable(CF_TEXT)) {									// Must check the format, otherwise can get a nice exception
		if (OpenClipboard(nullptr)) {
			str = std::string((char*)GetClipboardData(CF_TEXT));				// Get and convert the clipboard text to string
		}
		CloseClipboard();
	}
	return str;
}

void GLConsole::set_clipboard_text(std::string copy) {
	if (OpenClipboard(nullptr)) {												// If can open clipboard
		EmptyClipboard();														// Clear it
		HGLOBAL hg = GlobalAlloc(GMEM_MOVEABLE, copy.size() + 1);				// Allocate mem for copied string
		if (!hg) {																// Return if it was unsuccesful
			CloseClipboard();
			return;
		}
		memcpy(GlobalLock(hg), copy.c_str(), copy.size() + 1);					// Copy string to clipboard
		GlobalUnlock(hg);
		SetClipboardData(CF_TEXT, hg);
		GlobalFree(hg);
	}
	CloseClipboard();
}

void GLConsole::process_cout() {
	std::string buffer = cout.str();
	if (!buffer.empty()) {
		while (pos_scroll > 0) {
			this->scroll_down();
		}
		std::string line = "";
		for (auto c : buffer) {								// Divide the string to lines of strings
			switch (c) {
				case '\n':									// At the end of a line
					buffer_output.push_front(line);			// Put the line into the output buffer
					line = "";								// Clear the local variable for new lines
					break;
				case '\t':									// Handle tabs as 4 space
					line = line + "    ";
					break;
				default:
					line = line + c;						// Else just concat the char
			}
		}
		if (buffer_output.size() > size_buffer_output) {	// The buffer has a maximum length
			buffer_output.resize(size_buffer_output);		// If reached, resize it
		}
	}
	cout.str("");
	cout.clear();
}

void GLConsole::buffer_command_previous() {
	if (pos_buffer_command < (int)buffer_command.size() - 1 && buffer_command.size() > 0) {
		pos_buffer_command++;
		buffer_input = std::deque<unsigned char>(buffer_command[pos_buffer_command].begin(), buffer_command[pos_buffer_command].end());
		this->cursor_jump_right();
	}
}

void GLConsole::buffer_command_next() {
	if (pos_buffer_command > 0) {
		pos_buffer_command--;
		buffer_input = std::deque<unsigned char>(buffer_command[pos_buffer_command].begin(), buffer_command[pos_buffer_command].end());
		pos_cursor = buffer_command[pos_buffer_command].size();		// And place the cursor at the end of the text
	} else {
		pos_buffer_command = -1;									// Since 0 is the first element, -1 indicates the clear input line
		buffer_input.clear();										// Hence the clearing
		this->cursor_jump_left();									// And the cursor placing to the top left
	}
}

void GLConsole::cursor_move_left() {
	if (!selecting_buffer_input && pressed_shift) {					// The user just started the selecting_buffer_input
		pos_selection = pos_cursor;									// The position where the selection started
		selecting_buffer_input = true;
	}
	if (selecting_buffer_input && !pressed_shift) {					// When shift is not pressed, pressing the left arrow ends the selection
		pos_cursor = std::min(pos_selection, pos_cursor);			// By definiton, pressing the left arrow while selecting_buffer_input, places the cursor at the start of the selection
		selecting_buffer_input = false;
	} else if (pos_cursor > 0) {									// Else just moves the cursor to the left by one
		pos_cursor--;
	}
}

void GLConsole::cursor_move_right() {
	if (!selecting_buffer_input && pressed_shift) {					// The user just started selecting_buffer_input
		pos_selection = pos_cursor;									// The position where the selection started
		selecting_buffer_input = true;
	}
	if (selecting_buffer_input && !pressed_shift) {					// When shift is not pressed, pressing the right arrow ends the selection
		pos_cursor = std::max(pos_selection, pos_cursor);			// By definiton, pressing the right arrow while selecting_buffer_input, places the cursor at the end of the selection
		selecting_buffer_input = false;
	} else if (pos_cursor < buffer_input.size()) {					// Else just moves the cursor to the right by one
		pos_cursor++;
	}
}

void GLConsole::cursor_jump_left() {
	if (!selecting_buffer_input && pressed_shift) {					// The user just started selecting_buffer_input
		pos_selection = pos_cursor;									// The position where the selection started
		selecting_buffer_input = true;
	}
	if (selecting_buffer_input && !pressed_shift) {					// When shift is not pressed, pressing home key ends the selection
		selecting_buffer_input = false;
	}
	pos_cursor = 0;													// By definiton, pressing home places the cursor at the start of the text
}

void GLConsole::cursor_jump_right() {
	if (!selecting_buffer_input && pressed_shift) {					// The user just started the selecting_buffer_input
		pos_selection = pos_cursor;									// The position where selection started
		selecting_buffer_input = true;
	}
	if (selecting_buffer_input && !pressed_shift) {					// When shift is not pressed, pressing end keys ends the selection
		selecting_buffer_input = false;
	}
	pos_cursor = buffer_input.size();								// By definiton, pressing end places the cursor at the end of the text
}

void GLConsole::complete_command() {
	std::string command = std::string(buffer_input.begin(), buffer_input.end());	// Get command string from input buffer
	std::string partial = command;
	partial.erase(std::remove(partial.begin(), partial.end(), ' '), partial.end());	// Remove spaces
	partial = partial.substr(partial.find_last_of(';') + 1);
	command = command + cvars.complete(partial);									// Complete the command
	buffer_input = std::deque<unsigned char>(command.begin(), command.end());		// Write back to input buffer
	this->cursor_jump_right();
}

void GLConsole::process_commands() {
	while (!buffer_commands.empty() && buffer_commands_single.empty()) {					// When there are avalaiable commands, and al single command was executed
		std::string command = buffer_commands.front();										// Get the command string
		buffer_commands.pop_front();														// Remove it from the commands buffer

		if (buffer_command.empty() || buffer_command.front() != command) {					// Put it into the command history if it's empty or command differs from the last entered command
			buffer_command.push_front(command);
			if (buffer_command.size() > size_buffer_command) {								// Resize the command buffer if needed
				buffer_command.resize(size_buffer_command);
			}
		}
		pos_buffer_command = -1;															// No command selected in the command buffer

		cout << ">" << command << "\n";														// Print the command to the console
		buffer_input.clear();																// Clear the input buffer
		this->cursor_jump_left();															// And set the cursor tot he top left

		std::transform(command.begin(), command.end(), command.begin(), ::tolower);			// Convert command string to lowercase
		char chars[] = { ' ', '\t', '\n', '\r' };
		for (auto c : chars) {
			command.erase(std::remove(command.begin(), command.end(), c), command.end());	// Remove the above chars from the command
		}

		while (command.find(';') != std::string::npos) {									// Split command into commands by semicolon
			auto pos = command.find(';');
			buffer_commands_single.push_back(command.substr(0, pos));						// List of single, executable commands by the process_command method
			command = command.substr(pos + 1);
		}
	}

	if (watch_command.try_stop() && !buffer_commands_single.empty()) {
		std::string command = buffer_commands_single.front();
		buffer_commands_single.pop_front();
		this->process_command(command);
	}
}

void GLConsole::process_command(std::string command) {
	if (command == "help") {													// List the avalaible commands
		this->print_help();
	}

	else if (command == "list") {												// Lists the modifiable variables with their associated value
		cvars.print_tree(cout);
	}

	else if (command == "cls") {												// Completly clears the screen
		buffer_output.clear();
		//cout.str("");
	}

	else if (command.substr(0, 5) == "start") {
		std::string file_path = command.substr(5) + ".script";
		std::ifstream is = std::ifstream(file_path);
		if (is.good()) {
			for (std::string line; std::getline(is, line); ) {
				buffer_commands.push_back(line);
			}
		} else {
			cout << "Script does not exist!\n";
		}
	}

	else if (command.substr(0, 4) == "wait") {
		//std::this_thread::sleep_for(std::chrono::milliseconds(std::stoi(command.substr(4))));
		float secs = std::stof(command.substr(4));
		watch_command.start(secs);
	}

	else if (command == "reset") {												// Resets the console to the default state
		this->reset();
		cout << "The console has been reseted!\n";
	}

	else if (command == "exit") {												// Exits the application
		glutLeaveFullScreen();
		glutDestroyWindow(1);
		exit(0);
	}

	else if (command.find("()") != std::string::npos) {							// Call the function
		try {
			funcs.at(command)();
		} catch (...) {
			cout << "Function does not exist!\n";
		}
	}

	else if (command.find('=') != std::string::npos) {							// Sets a console variable
		auto pos = command.find('=');
		std::string name = command.substr(0, pos);								// Name of the variable
		std::string value = command.substr(pos + 1);							// Value to be set
		try {
			cvars.set_cvar(name, value);										// Possibility of not existing cvar
		} catch (char const* str) {
			cout << str << "\n";
		}
	}

	else try {																	// Gets the value of a variable
		CVarBase* node = cvars.find(command);
		cout << *node << "\n";
	} catch (char const* str) {
		cout << str << "\n";
	}
}