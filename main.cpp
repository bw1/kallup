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
// #include "all.h"

// ersatz für all.h
#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <cursesw.h>


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
	if (ypos-1 >= win_height) {	\
	ypos = win_height;        }

int main(int argc, char **argv)
{
	int ch, xpos = 0, ypos = 1, buffer_line = 0, index;
	int BUFFER_LINES = 0;

	// gib den versatz des bildausschnittes an 
	int yversatz=0;

	WINDOW *bg_win;
	WINDOW *my_win;

	setlocale(LC_CTYPE, "");

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

	buffer[0] = "dfjldfjklfoiucsvio lxcv lk xcvkl xjcvklxc\n";
	buffer[1] = " ij  jklj jklckl lkjyxc\n";
	buffer[2] = "\n";
	//buffer[3] = "3: kklÃ¶aksdklÃ¶aksdkadlsÃ¶klasÃ¶dasdlÃ¶kasdlkasd\n";
	buffer[3] = "3: kklöaksdklöaksdkadlsöklasödasdlökasdlkasd\n";
	buffer[4] = "4: kklöaksdklöaksdkadlsöklasödasdlökasdlkasd\n";
	buffer[5] = "5: kklöaksdklöaksdkadlsöklasödasdlökasdlkasd\n";

	for (int a=6; a<31 ; a++) {
		std::stringstream ss;
		ss << a;
		buffer[a] = ss.str() + ": adfasdfasdf\n";
	}
	BUFFER_LINES = 31;

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
				// kein loeschen in home position
				if (!(xpos == 0 && ypos ==1)) 
				{
					// in der ersten spalte (xpos ==0)
					if (xpos == 0) 
					{
						std::string st;
						int le;

						st= buffer[buffer_line-1];
						le= st.size()-1;
						st.erase(le,1);
						st= st + buffer[buffer_line];
						buffer[buffer_line-1] =st;

						buffer.erase(buffer.begin()+ buffer_line);

						buffer_line--;
						BUFFER_LINES--;

						xpos = le;
						ypos --;
					}
					else
					{
						// ein zeichen irgendwo in der zeile
						--xpos;
						CHECK_XPOS

						if (xpos == win_width-2)
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

						if (ypos < 1) ypos = 1;
						if (ypos == 1 && buffer_line == 1) --buffer_line;

						if (buffer[buffer_line].size() > 0)
							buffer[buffer_line].erase(xpos, 1);
					}
				}
			}
			break;

			case KEY_UP:
			{
				int tmp_xpos = xpos;

				--ypos;
				if (ypos == 0 && buffer_line > 0) yversatz--;

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
				if (ypos >win_height && buffer_line < BUFFER_LINES-1) yversatz++;


				CHECK_YPOS

				buffer_line	++;
				if (buffer_line >= BUFFER_LINES) 
				{
					buffer_line= BUFFER_LINES-1;
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

				if (xpos > 0)
				{
					it = buffer.begin();
					it = buffer.insert(it + buffer_line + 1,
						buffer[buffer_line].substr(xpos));

					buffer[buffer_line].replace(xpos, std::string::npos,"\n");
				}
				else 
				{
					it = buffer.begin();
					it = buffer.insert(it + buffer_line,"\n");
				}

				xpos = 0;

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
		for (int i = yversatz; i < BUFFER_LINES && i < win_height+yversatz; i++)
			mvwprintw(my_win,i-yversatz,0,"%s",buffer[i].c_str());


		mvwprintw(my_win,22,20,"%d, %d, %d, %d :: %d  =%d="
				,xpos, ypos, buffer_line, yversatz, win_width,ch);

		wmove(my_win,ypos-1,xpos);
		wrefresh(my_win);
	}

	endwin();

	for (int i = 0; i < BUFFER_LINES; i++)
		std::cout <<i << ": "<< buffer[i];


	return 0;
}
