/**
 * Copyright (C) 2015 NIPE-SYSTEMS
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef __CANVAS_LINECAP_H__
#define __CANVAS_LINECAP_H__

typedef enum
{
	CANVAS_LINE_CAP_BUTT = VG_CAP_BUTT,
	CANVAS_LINE_CAP_ROUND = VG_CAP_ROUND,
	CANVAS_LINE_CAP_SQUARE = VG_CAP_SQUARE
} canvas_line_cap_t;

void canvas_lineCap(char *line_cap);
char *canvas_lineCap_get(void);

#endif /* __CANVAS_LINECAP_H__ */
