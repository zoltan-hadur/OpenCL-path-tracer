#include "GLConsole.h"

#include <fstream>

std::map<std::string, std::function<void(std::vector<std::string>)>> GLConsole::funcs;	// Static variables need to be defined
ConsoleVariableManager GLConsole::cvars;										// Static variables need to be defined
std::ostringstream GLConsole::cout;									// Static variables need to be defined

GLConsole::GLConsole()
{
    // Does nothing
}

void GLConsole::init()
{
    glWindowPos2i = (PFNGLWINDOWPOS2IPROC)glutGetProcAddress("glWindowPos2i");	// Loads the function

    state = State::CLOSED;
    watch_render.Start();
    watch_command.Start();

    animl_rolling = 0.33f;
    cvars.Add(ConsoleVariable(&animl_rolling, "console.animations.rolling", "Time needed to roll down/up the console window in seconds. Interval: [0, infty).", std::bind(&GLConsole::animl_rolling_changed, this)));
    animl_interface = 0.25f;
    cvars.Add(ConsoleVariable(&animl_interface, "console.animations.interface", "Time needed for the console interface to appear/disappear in seconds. Interval: (0, infty).", std::bind(&GLConsole::animl_interface_changed, this)));
    animl_cursor = 1.0f;
    cvars.Add(ConsoleVariable(&animl_cursor, "console.animations.cursor", "Frequency of the cursor's blinking in Hz. Interval: [0.1, infty)."));
    acc_rolling = 0.0f;
    acc_interface = 0.0f;
    acc_cursor = 0.0f;
    overlap = 0.68f;
    cvars.Add(ConsoleVariable(&overlap, "console.overlap", "How much percent the console window takes from the screen. Interval: [0.2, 1]."));
    scroll_lines = 1;
    cvars.Add(ConsoleVariable(&scroll_lines, "console.buffers.scroll_lines", "Number of lines scrolled. Interval: [1, infty)."));
    color_background = Color(0, 0.5f, 1, 0.68f);
    cvars.Add(ConsoleVariable(&color_background, "console.colors.background", "Color of the console window in RGBA. Interval: [0, 1]."));
    color_interface = Color(1, 1, 1);
    cvars.Add(ConsoleVariable(&color_interface, "console.colors.interface", "Color of the console interface in RGBA. Interval: [0, 1]."));
    color_text = Color(1, 1, 1);
    cvars.Add(ConsoleVariable(&color_text, "console.colors.text", "Color of the text in RGBA. Interval: [0, 1]."));
    color_text_selection = Color(1, 0, 0);
    cvars.Add(ConsoleVariable(&color_text_selection, "console.colors.text_selection", "Color of the selected text in RGBA. Interval: [0, 1]."));
    pos_scrollbar = 0.0f;
    pos_scroll = 0;
    size_buffer_output = 1000;
    cvars.Add(ConsoleVariable(&size_buffer_output, "console.buffers.output", "The output buffer's size. Interval: [100, infty)."));
    buffer_output = std::deque<std::string>();
    pos_buffer_command = -1;
    size_buffer_command = 50;
    cvars.Add(ConsoleVariable(&size_buffer_command, "console.buffers.command", "The command buffer's size. Interval: [10, infty)."));
    buffer_command = std::deque<std::string>();
    pos_cursor = 0;
    pos_selection = 0;
    selecting_buffer_input = false;
    pressed_shift = false;
    buffer_input = std::deque<unsigned char>();
}

void GLConsole::reset()
{
    animl_rolling = 0.33f;
    animl_interface = 0.25f;
    animl_cursor = 1.0f;
    overlap = 0.68f;
    scroll_lines = 1;
    color_background = Color(0, 0.5f, 1, 0.68f);
    color_interface = Color(1, 1, 1);
    color_text = Color(1, 1, 1);
    color_text_selection = Color(1, 0, 0);
    size_buffer_output = 1000;
    pos_buffer_command = -1;
    size_buffer_command = 50;
    pos_cursor = 0;
    pos_selection = 0;
    selecting_buffer_input = false;
    pressed_shift = false;
    buffer_input.clear();
}

void GLConsole::open()
{
    if (state == State::CLOSED)
    {					// If the console is closed,
        state = State::ROLLING_DOWN;				// starts a ROLLING_DOWN->INTERFACE_APPEARING->OPENED process
    }
}

void GLConsole::close()
{
    if (state == State::OPENED)
    {					// If the console is opened,
        state = State::INTERFACE_DISAPPEARING;		// starts an INTERFACE_DISAPPEARING->ROLLING_UP->CLOSED process
    }
}

void GLConsole::toggle()
{
    if (state == State::OPENED)
    {					// If the console is opened,
        this->close();						// closes it
    }
    else if (state == State::CLOSED)
    {			// Else if the console is closed,
        this->open();						// opens it
    }
}

bool GLConsole::is_open()
{
    return state == State::OPENED;					// Returns true if the console is opened
}

void GLConsole::draw()
{
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    float dt = watch_render.GetDeltaTime();
    
    if (state == State::ROLLING_DOWN)
    {
        acc_rolling = std::min(acc_rolling + dt, animl_rolling);
        if (acc_rolling == animl_rolling)
        {
            state = State::INTERFACE_APPEARING;
        }
    }
    else if (state == State::ROLLING_UP)
    {
        acc_rolling = std::max(acc_rolling - dt, 0.0f);
        if (acc_rolling == 0.0f)
        {
            state = State::CLOSED;
        }
    }

    int width = glutGet(GLUT_WINDOW_WIDTH);
    int height = glutGet(GLUT_WINDOW_HEIGHT);

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glTranslatef(0, -height * overlap * (1 - acc_rolling / animl_rolling), 0);
    glColor4f(color_background.R, color_background.G, color_background.B, color_background.A);
    glBegin(GL_QUADS);
    glVertex2f(0, 0);
    glVertex2f(0, height * overlap);
    glVertex2f(width, height * overlap);
    glVertex2f(width, 0);
    glEnd();
    glPopMatrix();

    if (state == State::OPENED || state == State::INTERFACE_APPEARING || state == State::INTERFACE_DISAPPEARING)
    {
        this->draw_console(dt);				// Animate interface appearing/disapperaing and drawing text
    }
    glDisable(GL_BLEND);
}

void GLConsole::process()
{
    this->process_commands();				// Execute commands from the command list
    this->process_cout();					// Handle if something printed to the console
}

void GLConsole::on_keyboard(unsigned char key)
{
    switch (key)
    {
        case 3:		// Ctrl + c
            if (selecting_buffer_input)
            {																			// If selecting text
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
            if (selecting_buffer_input)
            {
                int from = std::min(pos_selection, pos_cursor);														// Selection starts at this index
                int to = std::max(pos_selection, pos_cursor);														// Selection ends at this index
                std::string selected_text = std::string(buffer_input.begin() + from, buffer_input.begin() + to);	// The selected text in the input buffer
                this->set_clipboard_text(selected_text);															// Copy it to the clipboard
                this->delete_char_before();																			// And remove it from the input buffer
            }
            break;
        case 9:		// Tab key
            this->complete_command();																				// Completes a ConsoleVariable's name
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

void GLConsole::on_special(int key)
{
    switch (key)
    {
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

void GLConsole::shift_pressed()
{
    pressed_shift = true;
}

void GLConsole::shift_released()
{
    pressed_shift = false;
}

void GLConsole::scroll_up()
{
    int width = glutGet(GLUT_WINDOW_WIDTH); int height = glutGet(GLUT_WINDOW_HEIGHT);	// Current size of window for handle too long lines
    int char_width = 8; int char_height = 14;											// Used font's size
    int max_output_lines = overlap * height / char_height - 1;							// Maximum number of lines that fits into the console

    int max_scroll_pos = this->handle_overflow(buffer_output, size_buffer_output).size() - max_output_lines;	// To don't "scroll out"
    if (pos_scroll < max_scroll_pos)
    {
        int new_scroll_position = std::min(max_scroll_pos, pos_scroll + scroll_lines);
        for (int i = 0; i < new_scroll_position - pos_scroll; ++i)
        {					// Scrolling is in fact is circshifting the output buffer
            buffer_output.push_back(buffer_output.front());
            buffer_output.pop_front();
        }
        pos_scroll = new_scroll_position;
    }
    pos_scrollbar = (float)pos_scroll / max_scroll_pos;									// Sets the scrollbar position
}

void GLConsole::scroll_down()
{
    int width = glutGet(GLUT_WINDOW_WIDTH); int height = glutGet(GLUT_WINDOW_HEIGHT);	// Current size of window for handle too long lines
    int char_width = 8; int char_height = 14;											// Used font's size
    int max_output_lines = overlap * height / char_height - 1;							// Maximum number of lines that fits into the console

    int max_scroll_pos = this->handle_overflow(buffer_output, size_buffer_output).size() - max_output_lines;	// To don't "scroll out"
    if (pos_scroll > 0)
    {
        int new_scroll_position = std::max(0, pos_scroll - scroll_lines);
        for (int i = 0; i < pos_scroll - new_scroll_position; ++i)
        {					// Scrolling is in fact is circshifting the output buffer
            buffer_output.push_front(buffer_output.back());
            buffer_output.pop_back();
        }
        pos_scroll = new_scroll_position;
    }
    pos_scrollbar = (float)pos_scroll / max_scroll_pos;									// Sets the scrollbar position
}

void GLConsole::print_help()
{
    cout << "----------------------------------HELP----------------------------------\n";
    cout << "Functions:\n";
    for (auto f : funcs)
    {
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

void GLConsole::add_function(std::string name, std::function<void(std::vector<std::string>)> f)
{
    funcs.insert({ name, f });
}


void GLConsole::animl_rolling_changed()
{
    acc_rolling = animl_rolling;
}

void GLConsole::animl_interface_changed()
{
    acc_interface = animl_interface;
}

void GLConsole::draw_console(float dt)
{
    // Variables
    int width = glutGet(GLUT_WINDOW_WIDTH); int height = glutGet(GLUT_WINDOW_HEIGHT);	// Current size of window for handle too long lines
    int console_x = 2; int console_y = (1.0f - overlap) * height + 4;					// Bottom left corner of console
    int char_width = 8; int char_height = 14;											// Used font's size
    int max_chars_in_line = (width - 2 * char_width) / char_width;						// Maximum number of characters per line that fits in to the window
    int max_output_lines = overlap * height / char_height - 1;							// Maximum number of lines that fits in to the console

    // Draw the background
    glColor4f(color_background.R, color_background.G, color_background.B, color_background.A);	// Set background color
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
    glColor4f(color_interface.R, color_interface.G, color_interface.B, color_interface.A * acc_interface);	// Sets color
    glWindowPos2i(console_x, console_y + input_overflow_shift);								// If no overflow, the cursor is at the bottom left side of the console by default
    glutBitmapCharacter(GLUT_BITMAP_8_BY_13, '>');											// The cursor is part of the interface
    for (int i = 0; i < input_lines.size(); ++i)
    {											// Start drawing the text in the input buffer
        glWindowPos2i(console_x + char_width, console_y + i * char_height);
        for (int j = 0; j < input_lines[i].size(); ++j)
        {									// It happens char by char to handle selection
            unsigned char c = input_lines[i][j];
            int b_pos = (input_lines.size() - 1 - i) * max_chars_in_line + j;				// Calculate a theoretical cursor position to know if the currently drawed char is selected
            int rasterpos[4];
            glGetIntegerv(GL_CURRENT_RASTER_POSITION, rasterpos);
            if (std::min(pos_selection, pos_cursor) <= b_pos && b_pos < std::max(pos_selection, pos_cursor) && selecting_buffer_input)
            {	// Check if char is in the selection
                this->draw_selection(rasterpos[0], rasterpos[1]);
            }
            glColor4f(color_text.R, color_text.G, color_text.B, color_text.A * acc_interface);
            glWindowPos2i(rasterpos[0], rasterpos[1]);
            glutBitmapCharacter(GLUT_BITMAP_8_BY_13, c);
        }
    }

    // Draw the cursor
    // The cursor is visible only in the first half
    glColor4f(color_interface.R, color_interface.G, color_interface.B, color_interface.A * acc_interface);
    acc_cursor = fmod(acc_cursor + dt, 1.0 / animl_cursor);
    if (acc_cursor < 1.0 / animl_cursor / 2)
    {
        int x_pos = console_x + (pos_cursor % max_chars_in_line + 1) * char_width;
        int y_pos = console_y - 2 + ((input_lines.size() - 1) - pos_cursor / max_chars_in_line) * char_height;
        glWindowPos2i(x_pos, y_pos);
        glutBitmapCharacter(GLUT_BITMAP_8_BY_13, '_');
    }

    // Draw buffer_output
    glColor4f(color_text.R, color_text.G, color_text.B, color_text.A * acc_interface);
    auto output_lines = this->handle_overflow(buffer_output, max_output_lines);	// Get only enough lines to fill the console window from output buffer
    for (int i = 0; i < output_lines.size(); ++i)
    {								// Print every line
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
    glColor4f(color_interface.R, color_interface.G, color_interface.B, color_interface.A * acc_interface);
    glVertex2f(start_x, start_y);							// Bottom left
    glVertex2f(start_x, height);							// Top left
    glVertex2f(start_x + 5, height);						// Top right
    glVertex2f(start_x + 5, start_y);						// Bottom right
    // Draw scrollbar slider
    int shift = (height - (start_y + 20)) * pos_scrollbar;	// Shift for the slider
    glColor4f(color_interface.R * 0.7f, color_interface.G * 0.7f, color_interface.B * 0.7f, color_interface.A * acc_interface);	// It's color is a little darker to be visible
    glVertex2f(start_x, start_y + shift);					// Bottom left
    glVertex2f(start_x, start_y + 20 + shift);				// Top left
    glVertex2f(start_x + 5, start_y + 20 + shift);			// Top right
    glVertex2f(start_x + 5, start_y + shift);				// Bottom right
    glEnd();
    glBegin(GL_LINE_LOOP);
    // Draw input line box
    start_x = 0; start_y = height * (1 - overlap);
    glColor4f(color_interface.R, color_interface.G, color_interface.B, color_interface.A * acc_interface);
    glVertex2f(start_x, start_y);														// Bottom left
    glVertex2f(start_x, start_y + input_lines.size() * char_height + 2);				// Top left
    glVertex2f(start_x + width - 1, start_y + input_lines.size() * char_height + 2);	// Top right
    glVertex2f(start_x + width - 1, start_y);											// Bottom right
    glEnd();
    glPopMatrix(); glMatrixMode(GL_PROJECTION); glPopMatrix(); glMatrixMode(GL_MODELVIEW);	// Load the previously saved matrices

    acc_interface = sqrt(acc_interface) * animl_interface;	// Scaling back
    if (state == State::INTERFACE_APPEARING)
    {
        acc_interface = std::min(acc_interface + dt, animl_interface);
        if (acc_interface == animl_interface)
        {
            state = State::OPENED;
        }
    }
    else if (state == State::INTERFACE_DISAPPEARING)
    {
        acc_interface = std::max(acc_interface - dt, 0.0f);
        if (acc_interface == 0.0f)
        {
            state = State::ROLLING_UP;
        }
    }
}

void GLConsole::draw_selection(int x, int y)
{
    int char_width = 8; int char_height = 15; y = y - 3;

    glMatrixMode(GL_PROJECTION); glPushMatrix(); glLoadIdentity();							// Save current projection matrix
    glOrtho(0.0, glutGet(GLUT_WINDOW_WIDTH), 0.0, glutGet(GLUT_WINDOW_HEIGHT), -1.0, 1.0);	// Transform it to able to draw in pixel coordinates
    glMatrixMode(GL_MODELVIEW); glPushMatrix(); glLoadIdentity();							// Save current modelview matrix
    glColor4f(color_text_selection.R, color_text_selection.G, color_text_selection.B, color_text_selection.A);
    glBegin(GL_QUADS);
    glVertex2f(x, y);
    glVertex2f(x, y + char_height);
    glVertex2f(x + char_width, y + char_height);
    glVertex2f(x + char_width, y);
    glEnd();
    glPopMatrix(); glMatrixMode(GL_PROJECTION);
    glPopMatrix(); glMatrixMode(GL_MODELVIEW);
}

std::deque<std::string> GLConsole::handle_overflow(std::deque<std::string>& lines_original, int max_lines)
{
    int width = glutGet(GLUT_WINDOW_WIDTH); int height = glutGet(GLUT_WINDOW_HEIGHT);	// Current size of window for handle too long lines
    int char_width = 8; int char_height = 14;											// Used font's size
    int max_chars_in_line = (width - 2 * char_width) / char_width;						// Maximum number of characters per line that fits in to the window

    std::deque<std::string> lines;
    for (auto line_original : lines_original)
    {											// Iterating through original lines of text
        std::deque<std::string> lines_overflowed;
        while (line_original.size() > max_chars_in_line)
        {								// Some lines are too long, so we divide them into multiple line
            std::string line_overflowed = line_original.substr(0, max_chars_in_line);	// Get string that as wide as the screen, so it can fit in it
            lines_overflowed.push_front(line_overflowed);								// Collect these overflowed lines into a separate buffer
            line_original = line_original.substr(line_overflowed.size());				// Remove the string that we got above
        }
        lines_overflowed.push_front(line_original);										// Push the last overflowed string into the buffer
        for (auto line_overflowed : lines_overflowed)
        {									// Iterate through them, and add them to the new buffer
            lines.push_back(line_overflowed);
            if (lines.size() >= max_lines)
            {											// But only max_lines line, becouse of finite resources
                return lines;
            }
        }
    }
    return lines;
}

std::deque<std::string> GLConsole::handle_overflow(std::deque<unsigned char>& line_original, int max_lines)
{
    auto dequeue = std::deque<std::string>({ std::string(buffer_input.begin(), buffer_input.end()) });
    return this->handle_overflow(dequeue, max_lines);
}

void GLConsole::delete_char_before()
{
    if (selecting_buffer_input)
    {												// Delete the whole selection when selecting_buffer_input
        buffer_input.erase(buffer_input.begin() + std::min(pos_selection, pos_cursor), buffer_input.begin() + std::max(pos_selection, pos_cursor));
        this->cursor_move_left();												// Then move the cursor to the top right of the selection
    }
    else if (buffer_input.size() > 0 && pos_cursor > 0)
    {						// Else delete the character right before the cursor
        buffer_input.erase(buffer_input.begin() + (--pos_cursor));
    }
}

void GLConsole::delete_char_after()
{
    if (selecting_buffer_input)
    {												// Delete the whole selection when selecting_buffer_input
        buffer_input.erase(buffer_input.begin() + std::min(pos_selection, pos_cursor), buffer_input.begin() + std::max(pos_selection, pos_cursor));
        this->cursor_move_left();												// Then move the cursor to the top left of the selection
    }
    else if (buffer_input.size() > 0 && pos_cursor < buffer_input.size())
    {	// Else delete the character right after the cursor
        buffer_input.erase(buffer_input.begin() + pos_cursor);
    }
}

void GLConsole::insert_char(unsigned char key)
{
    if (selecting_buffer_input)
    {												// WHen selecting_buffer_input
        this->delete_char_before();												// Delete the whole selection
    }
    buffer_input.insert(buffer_input.begin() + (pos_cursor++), key);			// Then insert the char into the input buffer
    selecting_buffer_input = false;
}

void GLConsole::insert_paste(std::string paste)
{
    if (selecting_buffer_input)
    {												// When selecting_buffer_input
        this->delete_char_before();												// Delete the whole selection
    }
    for (unsigned char c : this->get_clipboard_text())
    {						// Then insert the string into the input line
        this->insert_char(c);
    }
    selecting_buffer_input = false;
}

std::string GLConsole::get_clipboard_text()
{
    std::string str = "";
    if (IsClipboardFormatAvailable(CF_TEXT))
    {									// Must check the format, otherwise can get a nice exception
        if (OpenClipboard(nullptr))
        {
            str = std::string((char*)GetClipboardData(CF_TEXT));				// Get and convert the clipboard text to string
        }
        CloseClipboard();
    }
    return str;
}

void GLConsole::set_clipboard_text(std::string copy)
{
    if (OpenClipboard(nullptr))
    {												// If can open clipboard
        EmptyClipboard();														// Clear it
        HGLOBAL hg = GlobalAlloc(GMEM_MOVEABLE, copy.size() + 1);				// Allocate mem for copied string
        if (!hg)
        {																// Return if it was unsuccesful
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

void GLConsole::process_cout()
{
    std::string buffer = cout.str();
    if (!buffer.empty())
    {
        while (pos_scroll > 0)
        {
            this->scroll_down();
        }
        std::string line = "";
        for (auto c : buffer)
        {								// Divide the string to lines of strings
            switch (c)
            {
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
        if (buffer_output.size() > size_buffer_output)
        {	// The buffer has a maximum length
            buffer_output.resize(size_buffer_output);		// If reached, resize it
        }
    }
    cout.str("");
    cout.clear();
}

void GLConsole::buffer_command_previous()
{
    if (pos_buffer_command < (int)buffer_command.size() - 1 && buffer_command.size() > 0)
    {
        pos_buffer_command++;
        buffer_input = std::deque<unsigned char>(buffer_command[pos_buffer_command].begin(), buffer_command[pos_buffer_command].end());
        this->cursor_jump_right();
    }
}

void GLConsole::buffer_command_next()
{
    if (pos_buffer_command > 0)
    {
        pos_buffer_command--;
        buffer_input = std::deque<unsigned char>(buffer_command[pos_buffer_command].begin(), buffer_command[pos_buffer_command].end());
        pos_cursor = buffer_command[pos_buffer_command].size();		// And place the cursor at the end of the text
    }
    else
    {
        pos_buffer_command = -1;									// Since 0 is the first element, -1 indicates the clear input line
        buffer_input.clear();										// Hence the clearing
        this->cursor_jump_left();									// And the cursor placing to the top left
    }
}

void GLConsole::cursor_move_left()
{
    if (!selecting_buffer_input && pressed_shift)
    {					// The user just started the selecting_buffer_input
        pos_selection = pos_cursor;									// The position where the selection started
        selecting_buffer_input = true;
    }
    if (selecting_buffer_input && !pressed_shift)
    {					// When shift is not pressed, pressing the left arrow ends the selection
        pos_cursor = std::min(pos_selection, pos_cursor);			// By definiton, pressing the left arrow while selecting_buffer_input, places the cursor at the start of the selection
        selecting_buffer_input = false;
    }
    else if (pos_cursor > 0)
    {									// Else just moves the cursor to the left by one
        pos_cursor--;
    }
}

void GLConsole::cursor_move_right()
{
    if (!selecting_buffer_input && pressed_shift)
    {					// The user just started selecting_buffer_input
        pos_selection = pos_cursor;									// The position where the selection started
        selecting_buffer_input = true;
    }
    if (selecting_buffer_input && !pressed_shift)
    {					// When shift is not pressed, pressing the right arrow ends the selection
        pos_cursor = std::max(pos_selection, pos_cursor);			// By definiton, pressing the right arrow while selecting_buffer_input, places the cursor at the end of the selection
        selecting_buffer_input = false;
    }
    else if (pos_cursor < buffer_input.size())
    {					// Else just moves the cursor to the right by one
        pos_cursor++;
    }
}

void GLConsole::cursor_jump_left()
{
    if (!selecting_buffer_input && pressed_shift)
    {					// The user just started selecting_buffer_input
        pos_selection = pos_cursor;									// The position where the selection started
        selecting_buffer_input = true;
    }
    if (selecting_buffer_input && !pressed_shift)
    {					// When shift is not pressed, pressing home key ends the selection
        selecting_buffer_input = false;
    }
    pos_cursor = 0;													// By definiton, pressing home places the cursor at the start of the text
}

void GLConsole::cursor_jump_right()
{
    if (!selecting_buffer_input && pressed_shift)
    {					// The user just started the selecting_buffer_input
        pos_selection = pos_cursor;									// The position where selection started
        selecting_buffer_input = true;
    }
    if (selecting_buffer_input && !pressed_shift)
    {					// When shift is not pressed, pressing end keys ends the selection
        selecting_buffer_input = false;
    }
    pos_cursor = buffer_input.size();								// By definiton, pressing end places the cursor at the end of the text
}

void GLConsole::complete_command()
{
    std::string command = std::string(buffer_input.begin(), buffer_input.end());	// Get command string from input buffer
    std::string partial = command;
    partial.erase(std::remove(partial.begin(), partial.end(), ' '), partial.end());	// Remove spaces
    partial = partial.substr(partial.find_last_of(';') + 1);
    command = command + cvars.AutoComplete(partial);									// Complete the command
    buffer_input = std::deque<unsigned char>(command.begin(), command.end());		// Write back to input buffer
    this->cursor_jump_right();
}

void GLConsole::process_commands()
{
    while (!buffer_commands.empty() && buffer_commands_single.empty())
    {					// When there are avalaiable commands, and al single command was executed
        std::string command = buffer_commands.front();										// Get the command string
        buffer_commands.pop_front();														// Remove it from the commands buffer

        if (buffer_command.empty() || buffer_command.front() != command)
        {					// Put it into the command history if it's empty or command differs from the last entered command
            buffer_command.push_front(command);
            if (buffer_command.size() > size_buffer_command)
            {								// Resize the command buffer if needed
                buffer_command.resize(size_buffer_command);
            }
        }
        pos_buffer_command = -1;															// No command selected in the command buffer

        cout << ">" << command << "\n";														// Print the command to the console
        buffer_input.clear();																// Clear the input buffer
        this->cursor_jump_left();															// And set the cursor tot he top left

        std::transform(command.begin(), command.end(), command.begin(), ::tolower);			// Convert command string to lowercase
        char chars[] = { ' ', '\t', '\n', '\r' };
        for (auto c : chars)
        {
            command.erase(std::remove(command.begin(), command.end(), c), command.end());	// Remove the above chars from the command
        }

        while (command.find(';') != std::string::npos)
        {									// Split command into commands by semicolon
            auto pos = command.find(';');
            buffer_commands_single.push_back(command.substr(0, pos));						// List of single, executable commands by the process_command method
            command = command.substr(pos + 1);
        }
    }

    while ((!watch_command.IsRunning() || watch_command.TryStop()) && !buffer_commands_single.empty())
    {					// Executes commands until a wait command not called
        std::string command = buffer_commands_single.front();
        buffer_commands_single.pop_front();
        this->process_command(command);
    }
}

void GLConsole::process_command(std::string command)
{
    if (command == "help")
    {													// List the avalaible commands
        this->print_help();
    }

    else if (command == "list")
    {												// Lists the modifiable variables with their associated value
        cvars.Print(cout);
    }

    else if (command == "cls")
    {												// Completly clears the screen
        buffer_output.clear();
        //cout.str("");
    }

    else if (command.substr(0, 5) == "start")
    {
        std::string file_path = command.substr(5) + ".script";
        std::ifstream is = std::ifstream(file_path);
        if (is.good())
        {
            for (std::string line; std::getline(is, line); )
            {
                buffer_commands.push_back(line);
            }
        }
        else
        {
            cout << "Script does not exist!\n";
        }
    }

    else if (command.substr(0, 4) == "wait")
    {
        //std::this_thread::sleep_for(std::chrono::milliseconds(std::stoi(command.substr(4))));
        float secs = std::stof(command.substr(4));
        watch_command.Start(secs);
    }

    else if (command == "reset")
    {												// Resets the console to the default state
        this->reset();
        cout << "The console has been reseted!\n";
    }

    else if (command == "exit")
    {												// Exits the application
        glutLeaveFullScreen();
        glutDestroyWindow(1);
        exit(0);
    }

    else if (command.find("(") != std::string::npos)
    {							// Call the function
        std::string func_name = command.substr(0, command.find("("));
        std::string params = command.substr(func_name.size() + 1);
        params = params.substr(0, params.size() - 1);
        std::vector<std::string> func_params;

        std::string param = "";
        for (int i = 0; i < params.size(); ++i)
        {
            if (params[i] == ',')
            {
                func_params.push_back(param);
                param = "";
            }
            else
            {
                param = param + params[i];
                if (i == params.size() - 1)
                {
                    func_params.push_back(param);
                }
            }
        }
        bool executed = false;
        for (auto element : funcs)
        {
            if (element.first.find(func_name) != std::string::npos)
            {
                element.second(func_params);
                executed = true;
                break;
            }
        }
        if (!executed)
        {
            cout << "Function does not exist!\n";
        }
    }

    else if (command.find('=') != std::string::npos)
    {							// Sets a console variable
        auto pos = command.find('=');
        std::string name = command.substr(0, pos);								// Name of the variable
        std::string value = command.substr(pos + 1);							// Value to be set
        auto variable = cvars.Find(name);
        if (variable)
        {
            std::istringstream is(value);
            is >> *variable;
        }
        else
        {
            cout << "Console variable does not exist!\n";
        }
    }

    else
    {
        ConsoleVariableBase* node = cvars.Find(command);
        if (node)
        {
            cout << *node << "\n";
        }
        else
        {
            cout << "Console variable does not exist!\n";
        }
    }
}