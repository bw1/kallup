/*
   (c) Copyright 2013-2014  Kallup Software.
   All rights reserved.

   Written by Jens Kallup <jkallup@web.de>,

   This file is subject to the terms and conditions of the MIT License:

   Permission is hereby granted, free of charge, to any person
   obtaining a copy of this software and associated documentation
   files (the "Software"), to deal in the Software without restriction,
   including without limitation the rights to use, copy, modify, merge,
   publish, distribute, sublicense, and/or sell copies of the Software,
   and to permit persons to whom the Software is furnished to do so,
   subject to the following conditions:

   The above copyright notice and this permission notice shall be
   included in all copies or substantial portions of the Software.

   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
   EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
   MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
   IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
   CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
   TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
   SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
#define __MAIN_RUN__
#include "all.h"

#define CHECK_XPOS 			\
	if (xpos <= 0) {		\
	if (ypos <= 1)			\
	xpos = 0; else {		\
	xpos = win_width-2;		\
	--ypos; } } else		\
	if (xpos-1 >= win_width-2) {	\
	xpos = win_width-2;        }

#define CHECK_YPOS 			\
	if (ypos <= 1) {		\
	ypos = 1; } else		\
	if (ypos-1 >= win_height-2) {	\
	ypos = win_height-2;        }

int main(int argc, char **argv)
{
	int ch, xpos = 0, ypos = 1, buffer_line = 0, index;
	int BUFFER_LINES = 0;

	WINDOW *bg_win;
	WINDOW *my_win;

	std::vector<std::string> buffer(100);
	std::vector<std::string>::iterator it;

	initscr();
	noecho();
	refresh();

	int win_width  = 80;
	int win_height = 25;

	bg_win = newwin(win_height,win_width,0,0);
	my_win = subwin(bg_win,win_height-2,win_width-2,1,1);

	win_height -= 3;
	win_width  -= 3;

	box(bg_win, 0, 0);
	wrefresh(bg_win);

	wmove (my_win, 1, 1);
	keypad(my_win, TRUE);

	BUFFER_LINES = 4;

	buffer[0] = "dfjldfjklfoiucsvio lxcv lk xcvkl xjcvklxc\n";
	buffer[1] = " ij  jklj jklckl lkjyxc\n";
	buffer[2] = "\n";
	buffer[3] = "kklöaksdklöaksdkadlsöklasödasdlökasdlkasd\n";

	for (int i = 0; i < BUFFER_LINES; i++)
	mvwprintw(my_win,ypos+i-1,xpos,"%s",buffer[i].c_str());
	wmove(my_win,ypos-1,xpos);

	index = 1;

	while((ch = wgetch(my_win)) != 'q')
	{
		switch (ch)
		{
			case 22:
			case 0xff08:
			case 127:
			case KEY_DC:
			case KEY_BACKSPACE:
			{
				if (ypos <= 1)
				{
					ypos = 1;
				}

				if (--xpos < 0)
				{
					xpos = 0;
					if (--ypos <= 1) ypos = 1;
				}

				if (--BUFFER_LINES < 0)
				BUFFER_LINES = 1;

				if (--buffer_line-1 < 0)
				buffer_line = 0;

				if (xpos == 1 && ypos == 1) break;
				else	{
					if (buffer.size() > 0)
					buffer.erase(buffer.begin() + buffer_line);
				}
				
				if (xpos == win_width-2)
				{
					--buffer_line;
					--BUFFER_LINES;

					if (buffer[buffer_line].size() <= xpos)
					{
						for (int i = 0; i < buffer[buffer_line].size(); i++)
						{
							if (buffer[buffer_line].c_str()[i] == '\n')
							{
								xpos = i;
								break;
							}

							--xpos;
						}

						if (xpos < 0) xpos = 0;
						if (BUFFER_LINES > 0)
						buffer.erase(buffer.begin()+buffer_line-1);
					}
				}
			}
			break;

			case KEY_UP:
			{
				int tmp_xpos = xpos;

				--ypos;
				CHECK_YPOS

				--buffer_line;
				if (buffer_line < 0)
				buffer_line = 0;

				if (buffer[buffer_line].c_str()[0] == '\n')
				{
					xpos = 0;
					break;
				}
				else
				{
					for (int i = 0; i < buffer[buffer_line].size(); i++)
					{
						if (buffer[buffer_line].c_str()[i] == '\n')
						{
							xpos = i;
							break;
						}

						++xpos;
					}
				
					if (xpos > tmp_xpos)
					xpos = tmp_xpos;
				}
			}
			break;

			case KEY_DOWN:
			{
				int tmp_xpos = xpos;

				++ypos;
				CHECK_YPOS
				
				if (++buffer_line >= BUFFER_LINES)
				{
					if (ypos >= BUFFER_LINES)
					{
						--buffer_line;
						--ypos;
						break;
					}
				}

				if (buffer[buffer_line].c_str()[0] == '\n')
				xpos = 0;
				else {
					for (int i = 0; i < buffer[buffer_line].size(); i++)
					{
						if (buffer[buffer_line].c_str()[i] == '\n')
						{
							xpos = i;
							break;
						}

						--xpos;
					}

					if (xpos > tmp_xpos)
					xpos = tmp_xpos;
				}
			}
			break;

			case KEY_RIGHT:
				++xpos;
				CHECK_XPOS 

				if (buffer[buffer_line].c_str()[xpos-1] == '\n'
				||  buffer[buffer_line].c_str()[0] == '\n')
				{
					int tmp_ypos = ypos;

					++ypos;
					xpos = 0;
					++buffer_line;

					if (buffer_line == BUFFER_LINES)
					{
						buffer_line = BUFFER_LINES - 1;
						for (int i = 0; i < buffer[buffer_line].size(); i++)
						{
							if (buffer[buffer_line].c_str()[i] == '\n')
							{
								xpos = i;
								ypos = tmp_ypos;
								break;
							}
						}
					}
				}
			break;

			case KEY_LEFT:
				--xpos;
				CHECK_XPOS

				if (xpos <= 0)
				{
					xpos = 0;

					if (--buffer_line < 0)
					buffer_line = 1;

					if (buffer[buffer_line].c_str()[0] == '\n')
					--buffer_line;
				}
				else if (xpos == win_width-2)
				{
					--buffer_line;

					if (buffer[buffer_line].size() <= xpos)
					{
						for (int i = 0; i < buffer[buffer_line].size(); i++)
						{
							if (buffer[buffer_line].c_str()[i] == '\n')
							{
								xpos = i;
								break;
							}

							--xpos;
						}
					}
					if (xpos < 0) xpos = 0;
				}
			break;

			case KEY_END:
				xpos = buffer[buffer_line].size()-1;
			break;

			case KEY_HOME:
			case KEY_LL:
				xpos = 0;
			break;

			case '\n':
			{
				++ypos;
				xpos = 0;

				if (xpos > 0)
				{
					buffer[buffer_line].insert(xpos,"\n");
				}
				else {
					it = buffer.begin();
					it = buffer.insert(it + buffer_line,"\n");
				}

				++buffer_line;
				++BUFFER_LINES;
			}
			break;

			default:
			{
				++xpos;

				CHECK_YPOS
				CHECK_XPOS;

				char foo[4];
				sprintf(foo,"%lc",ch);
				buffer[buffer_line].insert(xpos-1, foo);
			}
			break;
		}

		wclear(my_win);
		for (int i = 0; i < BUFFER_LINES; i++)
		mvwprintw(my_win,i,0,"%s",buffer[i].c_str());


		mvwprintw(my_win,20,20,"%d, %d, %d, :: %d",xpos, ypos, buffer_line, win_width);

		wmove(my_win,ypos-1,xpos);
		wrefresh(my_win);
	}

	endwin();
	return 0;
}
