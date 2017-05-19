#pragma once

#include <GL\glew.h>
#include <GL\glext.h>
#include <GL\freeglut.h>
#include <vector>
#include <deque>
#include <map>
#include <sstream>
#include <atlstr.h >
#include <algorithm>
#include "CVarBase.h"
#include "CVar.h"
#include "Stopwatch.h"

// Basic console for OpenGL applications written in C++.
class GLConsole {
private:
	PFNGLWINDOWPOS2IPROC glWindowPos2i;			// Function that needs to be loaded dynamically. It's sets the raster position.
	enum State {								// States for rendering the console window.
		CLOSED, ROLLING_DOWN,
		INTERFACE_APPEARING, OPENED,
		INTERFACE_DISAPPEARING, ROLLING_UP
	};

	CVarBase cvars;								// Variables that are attached to normal variables, and vice versa modifiable from the console.
	State state;								// The state of the console (opened, closed, etc).
	Stopwatch watch;							// To get delta time in seconds.

	float animl_rolling;						// Time needed to roll down/up the console window in seconds.
	float animl_interface;						// Time needed for the console interface to appear/disappear in seconds.
	float animl_cursor;							// Frequency of the cursor's blinking in Hz.
	float acc_rolling;							// 
	float acc_interface;						// 
	float acc_cursor;							// 
	float overlap;								// How much percent (from 0.0 to 1.0) the console window takes from the screen.
	int scroll_lines;							// Number of lines scrolled when the mouse wheels move.
	float3 color_background;					// Color of the console window.
	float color_background_transparency;		// Transparency of the console window.
	float3 color_interface;						// Color of the console's interface.
	float3 color_text;							// Color of the text.
	float3 color_text_selection;				// Color of the selected text.
	float pos_scroll_bar;						// Position of the scroll bar
	int pos_scroll;								// Determines how much lines were shifted in the output buffer.
	int size_buffer_output;						// The output buffer's size.
	std::deque<std::string> buffer_output;		// Stores everything what was printed to the console.
	int pos_buffer_command;						// Index pointing to a command in the command buffer.
	int size_buffer_command;					// Command_history's size.
	std::deque<std::string> buffer_command;		// List with all the commands that the user previously entered.
	int pos_cursor;								// Position of the cursor to manipulate the input buffer.
	int pos_selection;							// Position of where the cursor was at when the selection started.
	bool selecting_buffer_input;				// True if user currently selecting characters from the input buffer.
	bool pressed_shift;							// True if shift is pressed.
	std::deque<unsigned char> buffer_input;		// Stores the characters that the user writes to the console. Also can be manipulated like in nearly every text editor.



	void roll_console(float dt);				// Renders the console window when it is opening (rolling down) or closing (rolling up).
	void draw_console(float dt);				// Renders the console window, interface and the text.
	void draw_selection(int x, int y);			// Draws quads upon selected text in the buffer_input.
	std::deque<std::string> handle_overflow(std::deque<std::string>& lines_original, int max_lines);	// Returns with a list of lines (strings) that fits into the console window.
	std::deque<std::string> handle_overflow(std::deque<unsigned char>& line_original, int max_lines);	// Returns with a list of lines (strings) that fits into the console window.
	void delete_char_before();					// Deletes the char before the cursor from the input buffer (like pressing backspace in a text editor).
	void delete_char_after();					// Deletes the char after the cursor from the input buffer (like pressing delete in a text editor).
	void insert_char(unsigned char key);		// Inserts a character at the cursor's position.
	void insert_paste(std::string paste);		// Inserts a whole string at the cursor's position.
	std::string get_clipboard_text();			// Returns with the string stored in the global clipboard (ctrl+v).
	void set_clipboard_text(std::string copy);	// Sets the global clipboard with the string selected in the input buffer (ctrl+c, ctrl+x).
	void handle_cout();							// Handle when something writes to the console using GLConsole::cout.
	void buffer_command_previous();				// Sets the input buffer to the previous command (like pressing the up arrow in Command Prompt).
	void buffer_command_next();					// Sets the input buffer to the next command (like pressing the down arrow in Command Prompt).
	void cursor_move_left();					// Moves the cursor to the left by one (like pressing left arrow in a text editor).
	void cursor_move_right();					// Moves the cursor to the right by one (like pressing the right arrow in a text editor).
	void cursor_jump_left();					// Moves the cursor to the top left (like pressing home in a text editor).
	void cursor_jump_right();					// Moves the cursor to the top right (like pressing end in a text editor).
	void complete_command();					// Completes the partially writed command in the input buffer (like pressing tab in Command Prompt).
	void process_command();						// Processes the command in the input buffer.
public:
	static std::ostringstream cout;				// Console out, works like std::cout.

	GLConsole();								// Do nothing. Must call init to work properly.
	void init();								// Initializing the console. Should only get called once, should only have one GLConsole instance and must be called after glutInit was called.
	void open_console();						// Opens the console window.
	void close_console();						// Closes the console window.
	void toggle_console();						// Opens the console window if it was closed, and vice versa.
	bool is_open();								// Returns true if console is open and usable.
	void render_console();						// Renders the whole console (animations, window, interface, text, everything), also handles the cout.
	void on_keyboard(unsigned char key);		// Keyboard event handler.
	void on_special(int key);					// Keyboard event handler for special keys (arrow, f1,2,3..., etc).
	void shift_pressed();						// Sets pressed_shift to true.
	void shift_released();						// Sets pressed_shift to false.
	void scroll_up();							// Scrolls up in the output text.
	void scroll_down();							// Scroll down in the output text.
	CVarBase& variables();						// Returns the data member, so one can use the console variables.
};

std::ostringstream GLConsole::cout;				// Static variables need to be defined.

GLConsole::GLConsole() {
	// Does nothing.
}

void GLConsole::init() {
	glWindowPos2i = (PFNGLWINDOWPOS2IPROC)glutGetProcAddress("glWindowPos2i");	// Loading the function.
	glEnable(GL_BLEND); glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);														// Enables transparency.

	cvars = CVarBase();
	state = CLOSED;
	watch = Stopwatch();

	animl_rolling = 0.33f;						cvars.attach_cvar<float>("console.animations.rolling", &animl_rolling, "Time needed to roll down/up the console window in seconds. Interval: [0, infty).");
	animl_interface = 0.25f;					cvars.attach_cvar<float>("console.animations.interface", &animl_interface, "Time needed for the console interface to appear/disappear in seconds. Interval: (0, infty).");
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
	pos_scroll_bar = 0.0f;
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

void GLConsole::open_console() {
	if (state == CLOSED) {					// If the console is closed,
		state = ROLLING_DOWN;				// starts a ROLLING_DOWN->INTERFACE_APPEARING->OPENED process.
	}
}

void GLConsole::close_console() {
	if (state == OPENED) {					// If the console is opened,
		state = INTERFACE_DISAPPEARING;		// starts an INTERFACE_DISAPPEARING->ROLLING_UP->CLOSED process.
	}
}

void GLConsole::toggle_console() {
	if (state == OPENED) {					// If the console is opened,
		this->close_console();				// closes it.
	} else if (state == CLOSED) {			// Else if the console is closed,
		this->open_console();				// opens it.
	}
}

bool GLConsole::is_open() {
	return state == OPENED;					// Return true if the console is opened.
}

void GLConsole::render_console() {
	float dt = watch.get_delta_time();		// Gets the elapsed time in seconds since the last call.
	this->handle_cout();					// Handle if something printed to the console.
	if (state == ROLLING_DOWN || state == ROLLING_UP) {
		this->roll_console(dt);				// Animate opening/closing.
	} else if (state == OPENED || state == INTERFACE_APPEARING || state == INTERFACE_DISAPPEARING) {
		this->draw_console(dt);				// Animate interface and draw text.
	}
}

void GLConsole::on_keyboard(unsigned char key) {
	switch (key) {
		case 3:		// Ctrl + c
			if (selecting_buffer_input) {
				// Copies string from input buffer and pastes it to the clipboard.
				this->set_clipboard_text(std::string(buffer_input.begin() + std::min(pos_selection, pos_cursor), buffer_input.begin() + std::max(pos_selection, pos_cursor)));
			}
			break;
		case 22:	// Ctrl + v
			// Pastes string from clipboard to the input buffer.
			this->insert_paste(this->get_clipboard_text());
			break;
		case 24:	// Ctrl + x
			if (selecting_buffer_input) {
				// Copies string from input buffer and pastes it to the clipboard while clearing the input buffer.
				this->set_clipboard_text(std::string(buffer_input.begin() + std::min(pos_selection, pos_cursor), buffer_input.begin() + std::max(pos_selection, pos_cursor)));
				this->delete_char_before();
			}
			break;
		case 9:		// Tab key
			this->complete_command();
			break;
		case 8:		// Backspace key
			this->delete_char_before();
			break;
		case 13:	// Enter key
			this->process_command();
			break;
		case 127:	// Delete key
			this->delete_char_after();
			break;
		default:	// Insert char at cursor
			this->shift_released();
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

CVarBase& GLConsole::variables() {
	return cvars;
}

void GLConsole::scroll_up() {
	int width = glutGet(GLUT_WINDOW_WIDTH); int height = glutGet(GLUT_WINDOW_HEIGHT);	// Current size of window for handle too long lines
	int char_width = 8; int char_height = 14;											// Used font's size
	int max_output_lines = overlap * height / char_height - 1;							// Maximum number of lines that fits into the console

	int max_scroll_pos = this->handle_overflow(buffer_output, size_buffer_output).size() - max_output_lines;	// To don't "scroll out"
	if (pos_scroll < max_scroll_pos) {
		int new_scroll_position = std::min(max_scroll_pos, pos_scroll + scroll_lines);
		for (int i = 0; i < new_scroll_position - pos_scroll; ++i) {					// Scrolling is in fact circshifting the output buffer
			buffer_output.push_back(buffer_output.front());
			buffer_output.pop_front();
		}
		pos_scroll = new_scroll_position;
	}
	pos_scroll_bar = (float)pos_scroll / max_scroll_pos;								// Sets the scroll bar position
}

void GLConsole::scroll_down() {
	int width = glutGet(GLUT_WINDOW_WIDTH); int height = glutGet(GLUT_WINDOW_HEIGHT);	// Current size of window for handle too long lines
	int char_width = 8; int char_height = 14;											// Used font's size
	int max_output_lines = overlap * height / char_height - 1;							// Maximum number of lines that fits into the console

	int max_scroll_pos = this->handle_overflow(buffer_output, size_buffer_output).size() - max_output_lines;	// To don't "scroll out"
	if (pos_scroll > 0) {
		int new_scroll_position = std::max(0, pos_scroll - scroll_lines);
		for (int i = 0; i < pos_scroll - new_scroll_position; ++i) {					// Scrolling is in fact circshifting the output buffer
			buffer_output.push_front(buffer_output.back());
			buffer_output.pop_back();
		}
		pos_scroll = new_scroll_position;
	}
	pos_scroll_bar = (float)pos_scroll / max_scroll_pos;								// Sets the scroll bar position
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
	// Draw the console's window according to the elapsed time since the start of the animation
	// acc_rolling == 0 -> the console window is not visible
	// acc_rolling == animl_rolling -> the console window is fully visible
	glColor4f(color_background[0], color_background[1], color_background[2], color_background_transparency);
	glBegin(GL_QUADS);
	glVertex2f(-1, 1);
	glVertex2f(1, 1);
	glVertex2f(1, 1 - 2 * (overlap * acc_rolling / animl_rolling));
	glVertex2f(-1, 1 - 2 * (overlap * acc_rolling / animl_rolling));
	glEnd();
}

void GLConsole::draw_console(float dt) {
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Variables
	int width = glutGet(GLUT_WINDOW_WIDTH); int height = glutGet(GLUT_WINDOW_HEIGHT);	// Current size of window for handle too long lines
	int console_x = 2; int console_y = (1.0f - overlap) * height + 4;					// Bottom left corner of console
	int char_width = 8; int char_height = 14;											// Used font's size
	int max_chars_in_line = (width - 2 * char_width) / char_width;						// Maximum number of characters per line that fits in to the window
	int max_output_lines = overlap * height / char_height - 1;							// Maximum number of lines that fits in to the console

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Draw the background
	glDisable(GL_LIGHTING);
	glColor4f(color_background[0], color_background[1], color_background[2], color_background_transparency);	// Set background color
	glBegin(GL_QUADS);
	glVertex2f(-1, 1);
	glVertex2f(1, 1);
	glVertex2f(1, 1 - 2 * overlap);
	glVertex2f(-1, 1 - 2 * overlap);
	glEnd();

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Draw the input line(s)
	// Interface/text transparency depends on acc_interface
	// acc_interface == 0 -> not visible
	// acc_interface == animl_interface -> visible
	acc_interface = pow(acc_interface / animl_interface, 2);
	auto input_lines = this->handle_overflow(buffer_input, max_output_lines);				// Get string from buffer
	int input_overflow_shift = (input_lines.size() - 1) * char_height;						// Determines the value needed to translate the raster position in pixels
	glColor4f(color_interface[0], color_interface[1], color_interface[2], acc_interface);	// Sets color
	glWindowPos2i(console_x, console_y + input_overflow_shift);								// If no overflow, it is at the bottom left side of the console by default
	glutBitmapCharacter(GLUT_BITMAP_8_BY_13, '>');											// Char '>' is part of the interface
	for (int i = 0; i < input_lines.size(); ++i) {											// Start drawing the text in the input buffer
		glWindowPos2i(console_x + char_width, console_y + i * char_height);					// It happens char by char to handle selection
		for (int j = 0; j < input_lines[i].size(); ++j) {
			unsigned char c = input_lines[i][j];
			int b_pos = (input_lines.size() - 1 - i) * max_chars_in_line + j;
			int rasterpos[4];
			glGetIntegerv(GL_CURRENT_RASTER_POSITION, rasterpos);
			if (std::min(pos_selection, pos_cursor) <= b_pos && b_pos < std::max(pos_selection, pos_cursor) && selecting_buffer_input) {
				this->draw_selection(rasterpos[0], rasterpos[1]);
			}
			glColor4f(color_text[0], color_text[1], color_text[2], acc_interface);
			glWindowPos2i(rasterpos[0], rasterpos[1]);
			glutBitmapCharacter(GLUT_BITMAP_8_BY_13, c);
		}
	}

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Draw the cursor
	glColor4f(color_interface[0], color_interface[1], color_interface[2], acc_interface);
	acc_cursor = fmod(acc_cursor + dt, animl_cursor);
	if (acc_cursor < animl_cursor / 2) {
		int x_pos = console_x + (pos_cursor % max_chars_in_line + 1) * char_width;
		int y_pos = console_y - 2 + ((input_lines.size() - 1) - pos_cursor / max_chars_in_line) * char_height;
		glWindowPos2i(x_pos, y_pos);
		glutBitmapCharacter(GLUT_BITMAP_8_BY_13, '_');
	}

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Draw buffer_output
	glColor4f(color_text[0], color_text[1], color_text[2], acc_interface);
	auto output_lines = this->handle_overflow(buffer_output, max_output_lines);
	for (int i = 0; i < output_lines.size(); ++i) {
		glWindowPos2i(console_x + char_width, console_y + input_overflow_shift + (i + 1) * char_height);
		glutBitmapString(GLUT_BITMAP_8_BY_13, (const unsigned char*)output_lines[i].c_str());
	}

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Draw scroll bar and input line box
	glMatrixMode(GL_PROJECTION); glPushMatrix(); glLoadIdentity();
	glOrtho(0.0, glutGet(GLUT_WINDOW_WIDTH), 0.0, glutGet(GLUT_WINDOW_HEIGHT), -1.0, 1.0);	// Draw in pixel coordinates
	glMatrixMode(GL_MODELVIEW); glPushMatrix(); glLoadIdentity();
	glBegin(GL_QUADS);
	int start_x = width - 5; int start_y = height * (1 - overlap) + input_lines.size() * char_height + 2;
	glColor4f(color_interface[0], color_interface[1], color_interface[2], acc_interface);
	glVertex2f(start_x, start_y);							// Bottom left
	glVertex2f(start_x, height);							// Top left
	glVertex2f(start_x + 5, height);						// Top right
	glVertex2f(start_x + 5, start_y);						// Bottom right
	int shift = (height - (start_y + 20)) * pos_scroll_bar;	// Shift for the scroll bar
	glColor4f(color_interface[0] * 0.7f, color_interface[1] * 0.7f, color_interface[2] * 0.7f, acc_interface);
	glVertex2f(start_x, start_y + shift);					// Bottom left
	glVertex2f(start_x, start_y + 20 + shift);				// Top left
	glVertex2f(start_x + 5, start_y + 20 + shift);			// Top right
	glVertex2f(start_x + 5, start_y + shift);				// Bottom right
	glEnd();
	glBegin(GL_LINE_LOOP);
	start_x = 0; start_y = height * (1 - overlap);
	glColor4f(color_interface[0], color_interface[1], color_interface[2], acc_interface);
	glVertex2f(start_x, start_y);														// Bottom left
	glVertex2f(start_x, start_y + input_lines.size() * char_height + 2);				// Top left
	glVertex2f(start_x + width - 1, start_y + input_lines.size() * char_height + 2);	// Top right
	glVertex2f(start_x + width - 1, start_y);											// Bottom right
	glEnd();
	glPopMatrix(); glMatrixMode(GL_PROJECTION);
	glPopMatrix(); glMatrixMode(GL_MODELVIEW);
	acc_interface = sqrt(acc_interface) * animl_interface;
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

	glMatrixMode(GL_PROJECTION); glPushMatrix(); glLoadIdentity();
	glOrtho(0.0, glutGet(GLUT_WINDOW_WIDTH), 0.0, glutGet(GLUT_WINDOW_HEIGHT), -1.0, 1.0);	// Draw in pixel coordinates
	glMatrixMode(GL_MODELVIEW); glPushMatrix(); glLoadIdentity();
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
			if (lines.size() >= max_lines)												// But only max_lines line, becouse of finite resources
				goto ret;																// Brr, goto, bad bad
		}
	}
ret:
	return lines;
}

std::deque<std::string> GLConsole::handle_overflow(std::deque<unsigned char>& line_original, int max_lines) {
	std::deque<std::string> tmp = { std::string(buffer_input.begin(), buffer_input.end()) };
	return this->handle_overflow(tmp, max_lines);
}

void GLConsole::delete_char_before() {
	if (selecting_buffer_input) {															// Delete the whole selection when selecting_buffer_input
		buffer_input.erase(buffer_input.begin() + std::min(pos_selection, pos_cursor), buffer_input.begin() + std::max(pos_selection, pos_cursor));
		this->cursor_move_left();												// Then move the cursor to the top right of the selection
	} else if (buffer_input.size() > 0 && pos_cursor > 0) {						// Else delete the character right before the cursor
		buffer_input.erase(buffer_input.begin() + (--pos_cursor));
	}
}

void GLConsole::delete_char_after() {
	if (selecting_buffer_input) {															// Delete the whole selection when selecting_buffer_input
		buffer_input.erase(buffer_input.begin() + std::min(pos_selection, pos_cursor), buffer_input.begin() + std::max(pos_selection, pos_cursor));
		this->cursor_move_left();												// Then move the cursor to the top left of the selection
	} else if (buffer_input.size() > 0 && pos_cursor < buffer_input.size()) {	// Else delete the character right after the cursor
		buffer_input.erase(buffer_input.begin() + pos_cursor);
	}
}

void GLConsole::insert_char(unsigned char key) {
	if (selecting_buffer_input) {													// WHen selecting_buffer_input
		this->delete_char_before();										// Delete the whole selection
	}
	buffer_input.insert(buffer_input.begin() + (pos_cursor++), key);	// Then insert the char intot he input line
	selecting_buffer_input = false;
}

void GLConsole::insert_paste(std::string paste) {
	if (selecting_buffer_input) {													// When selecting_buffer_input
		this->delete_char_before();										// Delete the whole selection
	}
	for (unsigned char c : this->get_clipboard_text()) {				// Then insert the string into the input line
		this->insert_char(c);
	}
	selecting_buffer_input = false;
}

std::string GLConsole::get_clipboard_text() {
	std::string str = "";
	if (IsClipboardFormatAvailable(CF_TEXT)) {
		if (OpenClipboard(nullptr)) {
			str = std::string((char*)GetClipboardData(CF_TEXT));
		}
		CloseClipboard();
	}
	return str;
}

void GLConsole::set_clipboard_text(std::string copy) {
	OpenClipboard(nullptr);
	EmptyClipboard();
	HGLOBAL hg = GlobalAlloc(GMEM_MOVEABLE, copy.size() + 1);
	if (!hg) {
		CloseClipboard();
		return;
	}
	memcpy(GlobalLock(hg), copy.c_str(), copy.size() + 1);
	GlobalUnlock(hg);
	SetClipboardData(CF_TEXT, hg);
	CloseClipboard();
	GlobalFree(hg);
}

void GLConsole::handle_cout() {
	if (!GLConsole::cout.str().empty()) {					// Copy printed text to output buffer
		while (pos_scroll > 0) {							// Scroll down completly to see the newly printed text
			this->scroll_down();
		}

		std::string line = "";
		for (auto c : GLConsole::cout.str()) {				// Divide the string to lines of strings
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
		GLConsole::cout.str("");							// Clear the stream
		if (buffer_output.size() > size_buffer_output) {	// The buffer has a maximum length
			buffer_output.resize(size_buffer_output);		// If reached, resize it
		}
	}
}

void GLConsole::buffer_command_previous() {
	if (pos_buffer_command < (int)buffer_command.size() - 1 && buffer_command.size() > 0) {
		pos_buffer_command++;
		buffer_input = std::deque<unsigned char>(buffer_command[pos_buffer_command].begin(), buffer_command[pos_buffer_command].end());
		pos_cursor = buffer_command[pos_buffer_command].size();
	}
}

void GLConsole::buffer_command_next() {
	if (pos_buffer_command > 0) {
		pos_buffer_command--;
		buffer_input = std::deque<unsigned char>(buffer_command[pos_buffer_command].begin(), buffer_command[pos_buffer_command].end());
		pos_cursor = buffer_command[pos_buffer_command].size();		// And place the cursor at the end of the text
	} else {
		pos_buffer_command = -1;										// Since 0 is the first element, -1 indicates the clear input line
		buffer_input.clear();											// Hence the clearing
		pos_cursor = 0;													// And the cursor placing to the top left
	}
}

void GLConsole::cursor_move_left() {
	if (!selecting_buffer_input && pressed_shift) {					// The user just started the selecting_buffer_input
		pos_selection = pos_cursor;						// The position where the selection started
		selecting_buffer_input = true;
	}
	if (selecting_buffer_input && !pressed_shift) {					// When shift is not pressed, pressing the left arrow ends the selection
		pos_cursor = std::min(pos_selection, pos_cursor);	// By definiton, pressing the left arrow while selecting_buffer_input, palces the cursor at the start of the selection
		selecting_buffer_input = false;
	} else if (pos_cursor > 0) {						// Else just moves the cursor to the left by one
		pos_cursor--;
	}
}

void GLConsole::cursor_move_right() {
	if (!selecting_buffer_input && pressed_shift) {					// The user just started selecting_buffer_input
		pos_selection = pos_cursor;						// The position where the selection started
		selecting_buffer_input = true;
	}
	if (selecting_buffer_input && !pressed_shift) {					// When shift is not pressed, pressing the right arrow ends the selection
		pos_cursor = std::max(pos_selection, pos_cursor);	// By definiton, pressing the right arrow while selecting_buffer_input, places the cursor at the end of the selection
		selecting_buffer_input = false;
	} else if (pos_cursor < buffer_input.size()) {		// Else just moves the cursor to the right by one
		pos_cursor++;
	}
}

void GLConsole::cursor_jump_left() {
	if (!selecting_buffer_input && pressed_shift) {	// The user just started selecting_buffer_input
		pos_selection = pos_cursor;		// The position where the selection started
		selecting_buffer_input = true;
	}
	if (selecting_buffer_input && !pressed_shift) {	// When shift is not pressed, pressing home key ends the selection
		selecting_buffer_input = false;
	}
	pos_cursor = 0;						// By definiton, pressing home places the cursor at the start of the text
}

void GLConsole::cursor_jump_right() {
	if (!selecting_buffer_input && pressed_shift) {	// The user just started the selecting_buffer_input
		pos_selection = pos_cursor;		// The position where selection started
		selecting_buffer_input = true;
	}
	if (selecting_buffer_input && !pressed_shift) {	// When shift is not pressed, pressing end keys ends the selection
		selecting_buffer_input = false;
	}
	pos_cursor = buffer_input.size();	// By definiton, pressing end places the cursor at the end of the text
}

void GLConsole::complete_command() {
	std::string command = std::string(buffer_input.begin(), buffer_input.end());

	command = cvars.complete(command);

	buffer_input = std::deque<unsigned char>(command.begin(), command.end());
	this->cursor_jump_right();
}

void GLConsole::process_command() {
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Get the command string
	std::string command = std::string(buffer_input.begin(), buffer_input.end());

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Put the command into the command history
	buffer_command.push_front(command);
	if (buffer_command.size() > size_buffer_command) {
		buffer_command.resize(size_buffer_command);
	}
	pos_buffer_command = -1;


	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Write the command to the output
	GLConsole::cout << ">" << command << '\n';
	std::transform(command.begin(), command.end(), command.begin(), ::tolower);
	pos_cursor = 0;
	buffer_input.clear();


	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// List the avalaible commands
	if (command == "help") {
		GLConsole::cout << "help\nexit\nlist\n";
	}

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Exit the application
	else if (command == "exit") {
		glutLeaveFullScreen();
		glutDestroyWindow(1);
		exit(0);
	}

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// List the modifiable variables with their associated value
	else if (command == "list") {
		cvars.print_tree(GLConsole::cout);
	}

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Set the value of a variable
	else if (command.find(" = ") != std::string::npos) {
		std::string name = command.substr(0, command.find(" = "));
		std::string value = command.substr(command.find(" = ") + 3);
		try {
			cvars.set_cvar(name, value);
		} catch (char const* str) {
			GLConsole::cout << str << "\n";
		}
	}

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Get the value of a variable
	else try {
		CVarBase* node = cvars.find(command);
		GLConsole::cout << *node << std::endl;
	} catch (char const* str) {
		GLConsole::cout << str << "\n";
	}
}