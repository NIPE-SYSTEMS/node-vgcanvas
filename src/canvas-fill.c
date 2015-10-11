/**
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
#include "canvas-fillStyle.h"
#include "canvas-fill.h"
#include "canvas-shadowColor.h"
#include "canvas-shadowBlur.h"
#include "canvas-shadowOffsetX.h"
#include "canvas-shadowOffsetY.h"

/**
 * The fill() method fills the current or given path with the current fill style
 * using the non-zero or even-odd winding rule.
 */
void canvas_fill(void)
{
	if(canvas_shadowColor_get_enabled())
	{
		egl_blur_begin();
		
		paint_activate(canvas_shadowColor_get(), VG_FILL_PATH);
		
		vgDrawPath(canvas_beginPath_get(), VG_FILL_PATH);
		
		egl_blur_end(canvas_shadowBlur_get(), canvas_shadowOffsetX_get(), canvas_shadowOffsetY_get());
	}
	
	paint_activate(canvas_fillStyle_get(), VG_FILL_PATH);
	
	vgDrawPath(canvas_beginPath_get(), VG_FILL_PATH);
}
