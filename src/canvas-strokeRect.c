/*
 * Copyright (C) 2015 NIPE-SYSTEMS
 * Copyright (C) 2015 Hauke Oldsen
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

#include "include-core.h"
#include "include-openvg.h"
// #include "include-freetype.h"

#include "egl-util.h"
#include "canvas-beginPath.h"
#include "canvas-paint.h"
#include "canvas-strokeStyle.h"
#include "canvas-strokeRect.h"
#include "canvas-moveTo.h"
#include "canvas-lineTo.h"
#include "canvas-closePath.h"

/**
 * The strokeRect() method paints a rectangle which has a starting point at (x,
 * y) and has a w width and an h height onto the canvas, using the current
 * stroke style.
 * @param x The x axis of the coordinate for the rectangle starting point.
 * @param y The y axis of the coordinate for the rectangle starting point.
 * @param width The rectangle's width.
 * @param height The rectangle's height.
 */
void canvas_strokeRect(VGfloat x, VGfloat y, VGfloat width, VGfloat height)
{
	paint_activate(canvas_strokeStyle_get(), VG_STROKE_PATH);
	
	canvas_beginPath();
	
	//vguRect(canvas_beginPath_get(), x, egl_get_height() - y - height, width, height);
	canvas_moveTo(x, y);
	canvas_lineTo(x + width, y);
	canvas_lineTo(x + width, y + height);
	canvas_lineTo(x, y + height);
	canvas_closePath();
	
	vgDrawPath(canvas_beginPath_get(), VG_STROKE_PATH);
}
