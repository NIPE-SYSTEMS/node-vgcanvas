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

extern "C" {
	#include "egl-util.h"
	#include "include-freetype.h"
	#include "include-openvg.h"
	#include "font-util.h"
	#include "log-util.h"
	#include "image-util.h"
	#include "canvas.h"
	#include "canvas-font.h"
	#include "canvas-paint.h"
	#include "canvas-fillRect.h"
	#include "canvas-arc.h"
	#include "canvas-beginPath.h"
	#include "canvas-bezierCurveTo.h"
	#include "canvas-clearRect.h"
	#include "canvas-clip.h"
	#include "canvas-closePath.h"
	#include "canvas-fill.h"
	#include "canvas-fillStyle.h"
	#include "canvas-fillText.h"
	#include "canvas-font.h"
	#include "canvas-globalAlpha.h"
	#include "canvas-lineCap.h"
	#include "canvas-lineDashOffset.h"
	#include "canvas-lineJoin.h"
	#include "canvas-lineTo.h"
	#include "canvas-lineWidth.h"
	#include "canvas-miterLimit.h"
	#include "canvas-moveTo.h"
	#include "canvas-quadraticCurveTo.h"
	#include "canvas-rect.h"
	#include "canvas-restore.h"
	#include "canvas-save.h"
	#include "canvas-setLineDash.h"
	#include "canvas-stroke.h"
	#include "canvas-strokeRect.h"
	#include "canvas-strokeStyle.h"
	#include "canvas-strokeText.h"
	#include "canvas-fillText.h"
	#include "canvas-strokeText.h"
	#include "canvas-drawImage.h"
	#include "canvas-imageSmoothingEnabled.h"
	#include "canvas-globalCompositeOperation.h"
	#include "canvas-textAlign.h"
	#include "canvas-textBaseline.h"
	#include "canvas-resetTransform.h"
	#include "canvas-rotate.h"
	#include "canvas-scale.h"
	#include "canvas-transform.h"
	#include "canvas-setTransform.h"
	#include "canvas-translate.h"
	#include "canvas-measureText.h"
}

#include <nan.h>
#include <string>
#include <map>
#include <cstdio>
#include "gradient.h"
#include "image.h"
#include "pattern.h"

using namespace v8;

namespace vgcanvas {

	static std::map<uint32_t, paint_t*> paintMap;
	static bool initialized = false;

	bool checkArgs(const Nan::FunctionCallbackInfo<Value> &args, int number, int offset) {

		if(args.Length() < number + offset) {
			Nan::ThrowTypeError("Not enough args");
			return false;
		}

		for(int i = offset; i < offset + number; i++) {
			if(!args[i]->IsNumber()) {
				Nan::ThrowTypeError("Wrong arg");
				return false;
			}

		}

		return true;

	}

	bool checkArgs(const Nan::FunctionCallbackInfo<Value> &args, int number) {
		return checkArgs(args, number, 0);
	}

	void ErrorHandler(const char *location, const char *message) {
		eprintf("V8 FATAL ERROR: %s %s\n", location, message);
		
		if(initialized) {
			initialized = false;
			canvas__cleanup();
		}
	}

	void Init(const Nan::FunctionCallbackInfo<Value>& args) {
		if(initialized) {
			Nan::ThrowError("Only one canvas can be created at the same time");
			return;
		}
		
		args.GetIsolate()->SetFatalErrorHandler(ErrorHandler);
		canvas__init();
		initialized = true;
	}

	void SwapBuffers(const Nan::FunctionCallbackInfo<Value>& args) {
		egl_swap_buffers();
	}

	void Cleanup(const Nan::FunctionCallbackInfo<Value>& args) {
		if(!initialized) {
			Nan::ThrowError("Not initialized");
			return;
		}
		
		canvas__cleanup();
		initialized = false;
	}

	void FillRect(const Nan::FunctionCallbackInfo<Value>& args) {
		if(!checkArgs(args, 4)) {
			return;
		}

		canvas_fillRect(args[0]->NumberValue(), args[1]->NumberValue(), args[2]->NumberValue(), args[3]->NumberValue());
	}

	void ClearRect(const Nan::FunctionCallbackInfo<Value>& args) {
		if(!checkArgs(args, 4)) {
			return;
		}

		canvas_clearRect(args[0]->NumberValue(), args[1]->NumberValue(), args[2]->NumberValue(), args[3]->NumberValue());
	}

	void SetFillStyle(const Nan::FunctionCallbackInfo<Value>& args) {
		if(!checkArgs(args, 4)) {
			return;
		}

		//canvas_fillStyle_color(args[0]->NumberValue(), args[1]->NumberValue(), args[2]->NumberValue(), args[3]->NumberValue());
	}

	void GetFillStyle(const Nan::FunctionCallbackInfo<Value>& args) {
		/*color_t color = canvas_getState()->fillColor;

		Local<Array> array = Nan::New<Array>(4);

		array->Set(0, Nan::New(color.red));
		array->Set(1, Nan::New(color.green));
		array->Set(2, Nan::New(color.blue));
		array->Set(3, Nan::New(color.alpha));

		args.GetReturnValue().Set(array);*/
	}

	void GetScreenWidth(const Nan::FunctionCallbackInfo<Value>& args) {
		args.GetReturnValue().Set(Nan::New(egl_get_width()));
	}

	void GetScreenHeight(const Nan::FunctionCallbackInfo<Value>& args) {
		args.GetReturnValue().Set(Nan::New(egl_get_height()));
	}

	void SetLineWidth(const Nan::FunctionCallbackInfo<Value>& args) {
		if(!checkArgs(args, 1)) {
			return;
		}

		canvas_lineWidth(args[0]->NumberValue());
	}

	void GetLineWidth(const Nan::FunctionCallbackInfo<Value>& args) {
		args.GetReturnValue().Set(Nan::New(canvas_lineWidth_get()));
	}

	void SetLineCap(const Nan::FunctionCallbackInfo<Value>& args) {
		if(args.Length() != 1 || !args[0]->IsString()) {
			Nan::ThrowTypeError("wrong arg");
			return;
		}

		canvas_lineCap(*Nan::Utf8String(args[0]));
	}

	void GetLineCap(const Nan::FunctionCallbackInfo<Value>& args) {
		args.GetReturnValue().Set(Nan::New(canvas_lineCap_get()).ToLocalChecked());
	}

	void SetLineJoin(const Nan::FunctionCallbackInfo<Value>& args) {
		if(args.Length() != 1 || !args[0]->IsString()) {
			Nan::ThrowTypeError("wrong arg");
			return;
		}

		canvas_lineJoin(*Nan::Utf8String(args[0]));

	}

	void GetLineJoin(const Nan::FunctionCallbackInfo<Value>& args) {
		args.GetReturnValue().Set(Nan::New(canvas_lineJoin_get()).ToLocalChecked());
	}

	void SetStyle(const Nan::FunctionCallbackInfo<Value>& args) {
		if(args.Length() != 2 || !args[0]->IsBoolean() || !args[1]->IsObject()) {
			Nan::ThrowTypeError("wrong arg");
			return;
		}
		
		bool stroke = Local<Boolean>::Cast(args[0])->Value();

		if(args[1]->IsArray()) {
			Local<Array> ar = Local<Array>::Cast(args[1]);
			if(ar->Length() != 4) {
				Nan::ThrowTypeError("wrong number of elements");
				return;
			}

			paint_t *paint = stroke ? canvas_strokeStyle_get() : canvas_fillStyle_get();
			if(paint->paint_type == PAINT_TYPE_COLOR) {
				paint_setRGBA(paint, ar->Get(0)->NumberValue(), ar->Get(1)->NumberValue(),
					ar->Get(2)->NumberValue(), ar->Get(3)->NumberValue());
			} else {
				// paint type is gradient
				paint = new paint_t;
				paint_createColor(paint, ar->Get(0)->NumberValue(), ar->Get(1)->NumberValue(),
					ar->Get(2)->NumberValue(), ar->Get(3)->NumberValue());
				
				if(stroke) {
					canvas_strokeStyle(paint);
				} else {
					canvas_fillStyle(paint);
				}
				
			}
		} else {
			//Gradient or pattern object
			paint_t *paint = stroke ? canvas_strokeStyle_get() : canvas_fillStyle_get();
			if(paint->paint_type == PAINT_TYPE_COLOR) {
				paint_cleanup(paint);
				delete paint;
			}

			Local<Object> obj = Local<Object>::Cast(args[1]);
			std::string constructor(*Nan::Utf8String(obj->GetConstructorName()));

			if(constructor == "Gradient") {
				Gradient *gr = Gradient::Unwrap<Gradient>(obj);
				if(stroke) {
					canvas_strokeStyle(gr->GetPaint());
				} else {
					canvas_fillStyle(gr->GetPaint());
				}
			} else if(constructor == "Pattern") {
				Pattern *pattern = Pattern::Unwrap<Pattern>(obj);
				if(stroke) {
					canvas_strokeStyle(pattern->GetPaint());
				} else {
					canvas_fillStyle(pattern->GetPaint());
				}
			} else {
				Nan::ThrowTypeError("type of object is neither Gradient nor Pattern");
				return;
			}

		}

	}
	
	void GetStrokeStyle(const Nan::FunctionCallbackInfo<Value>& args) {
		/*color_t color = canvas_getState()->strokeColor;

		Local<Array> array = Nan::New<Array>(4);

		array->Set(0, Nan::New(color.red));
		array->Set(1, Nan::New(color.green));
		array->Set(2, Nan::New(color.blue));
		array->Set(3, Nan::New(color.alpha));

		args.GetReturnValue().Set(array);*/
	}

	void StrokeRect(const Nan::FunctionCallbackInfo<Value>& args) {
		if(!checkArgs(args, 4)) {
			return;
		}

		canvas_strokeRect(args[0]->NumberValue(), args[1]->NumberValue(), args[2]->NumberValue(), args[3]->NumberValue());

	}

	void BeginPath(const Nan::FunctionCallbackInfo<Value>& args) {
		canvas_beginPath();
	}

	void ClosePath(const Nan::FunctionCallbackInfo<Value>& args) {
		canvas_closePath();
	}

	void MoveTo(const Nan::FunctionCallbackInfo<Value>& args) {
		if(!checkArgs(args, 2)) {
			return;
		}

		canvas_moveTo(args[0]->NumberValue(), args[1]->NumberValue());
	}

	void LineTo(const Nan::FunctionCallbackInfo<Value>& args) {
		if(!checkArgs(args, 2)) {
			return;
		}

		canvas_lineTo(args[0]->NumberValue(), args[1]->NumberValue());
	}

	void Stroke(const Nan::FunctionCallbackInfo<Value>& args) {
		canvas_stroke();
	}

	void Fill(const Nan::FunctionCallbackInfo<Value>& args) {
		canvas_fill();
	}

	void QuadraticCurveTo(const Nan::FunctionCallbackInfo<Value>& args) {
		if(!checkArgs(args, 4)) {
			return;
		}

		canvas_quadraticCurveTo(args[0]->NumberValue(), args[1]->NumberValue(), args[2]->NumberValue(), args[3]->NumberValue());
	}

	void BezierCurveTo(const Nan::FunctionCallbackInfo<Value>& args) {
		if(!checkArgs(args, 6)) {
			return;
		}

		canvas_bezierCurveTo(args[0]->NumberValue(), args[1]->NumberValue(), args[2]->NumberValue(), args[3]->NumberValue(), args[4]->NumberValue(), args[5]->NumberValue());
	}

	void Arc(const Nan::FunctionCallbackInfo<Value>& args) {
		if(!checkArgs(args, 5)) {
			return;
		}

		bool acw = false;
		if(args.Length() > 5 && args[5]->IsBoolean()) {
			acw = args[5]->BooleanValue();
		}

		canvas_arc(args[0]->NumberValue(), args[1]->NumberValue(), args[2]->NumberValue(), args[3]->NumberValue(), args[4]->NumberValue(), acw);
	}

	void Rect(const Nan::FunctionCallbackInfo<Value>& args) {
		if(!checkArgs(args, 4)) {
			return;
		}

		canvas_rect(args[0]->NumberValue(), args[1]->NumberValue(), args[2]->NumberValue(), args[3]->NumberValue());
	}

	void SetLineDash(const Nan::FunctionCallbackInfo<Value>& args) {
		if(args.Length() != 1 || !args[0]->IsArray()) {
			Nan::ThrowTypeError("wrong arg");
			return;
		}

		Local<Array> ar = Local<Array>::Cast(args[0]);

		VGfloat *data = new VGfloat[ar->Length()];

		for(uint32_t i = 0; i < ar->Length(); i++) {
			data[i] = ar->Get(i)->NumberValue();
		}

		canvas_setLineDash(ar->Length(), data);
		delete data;
	}

	void GetLineDash(const Nan::FunctionCallbackInfo<Value>& args) {
		VGfloat *pattern = canvas_setLineDash_get_data();
		VGint count = canvas_setLineDash_get_count();

		Local<Array> array = Nan::New<Array>(count);

		for(int i = 0; i < count; i++) {
			array->Set(i, Nan::New(pattern[i]));
		}

		args.GetReturnValue().Set(array);
	}

	void SetLineDashOffset(const Nan::FunctionCallbackInfo<Value>& args) {
		if(!checkArgs(args, 1)) {
			return;
		}

		canvas_lineDashOffset(args[0]->NumberValue());
	}

	void GetLineDashOffset(const Nan::FunctionCallbackInfo<Value>& args) {
		args.GetReturnValue().Set(Nan::New(canvas_lineDashOffset_get()));
	}

	void Clip(const Nan::FunctionCallbackInfo<Value>& args) {
		canvas_clip();
	}

	void Save(const Nan::FunctionCallbackInfo<Value>& args) {
		canvas_save();
	}

	void Restore(const Nan::FunctionCallbackInfo<Value>& args) {
		canvas_restore();
	}

	void SetGlobalAlpha(const Nan::FunctionCallbackInfo<Value>& args) {
		if(!checkArgs(args, 1)) {
			return;
		}

		canvas_globalAlpha(args[0]->NumberValue());
	}

	void GetGlobalAlpha(const Nan::FunctionCallbackInfo<Value>& args) {
		args.GetReturnValue().Set(Nan::New(canvas_globalAlpha_get()));
	}
	
	void NewFont(const Nan::FunctionCallbackInfo<Value>& args) {
		if(args.Length() < 2 || !args[0]->IsString(), !args[1]->IsString()) {
			Nan::ThrowTypeError("wrong arg");
			return;
		}
		
		if(font_util_new(*Nan::Utf8String(args[0]), *Nan::Utf8String(args[1])) == -1) {
			std::string msg = "Failed to create font: ";
			msg += strerror(errno);
			Nan::ThrowError(msg.c_str());
		}
	}
	
	void SetFont(const Nan::FunctionCallbackInfo<Value>& args) {
		if(!checkArgs(args, 1, 0) || !args[1]->IsString()) {
			Nan::ThrowTypeError("wrong args");
			return;
		}
		
		canvas_font(*Nan::Utf8String(args[1]), args[0]->NumberValue());
	}
	
	void FillText(const Nan::FunctionCallbackInfo<Value>& args) {
		if(!checkArgs(args, 2, 1)) {
			Nan::ThrowTypeError("wrong args");
			return;
		}
		
		canvas_fillText(*Nan::Utf8String(args[0]), args[1]->NumberValue(), args[2]->NumberValue());
	}
	
	void StrokeText(const Nan::FunctionCallbackInfo<Value>& args) {
		if(!checkArgs(args, 2, 1)) {
			Nan::ThrowTypeError("wrong args");
			return;
		}
		
		canvas_strokeText(*Nan::Utf8String(args[0]), args[1]->NumberValue(), args[2]->NumberValue());
	}
	
	void DrawImage(const Nan::FunctionCallbackInfo<Value>& args) {
		if(!args[0]->IsObject() || !checkArgs(args, 8, 1)) {
			Nan::ThrowTypeError("wrong args");
			return;
		}
		
		Image *img = Image::Unwrap<Image>(Local<Object>::Cast(args[0]));
		
		if(img->GetImage()) {
			canvas_drawImage(img->GetImage(), args[1]->NumberValue(), args[2]->NumberValue(), args[3]->NumberValue(), 
				args[4]->NumberValue(), args[5]->NumberValue(), args[6]->NumberValue(), args[7]->NumberValue(), args[8]->NumberValue());
		} else {
			Nan::ThrowError("invalid image");
			return;
		}
		
	}
	
	
	void SetImageSmoothing(const Nan::FunctionCallbackInfo<Value>& args) {
		if(args.Length() != 1 || !args[0]->IsBoolean()) {
			Nan::ThrowTypeError("wrong arg");
			return;
		}
		
		canvas_imageSmoothingEnabled(args[0]->BooleanValue() ? VG_TRUE : VG_FALSE);
	}
	
	void GetImageSmoothing(const Nan::FunctionCallbackInfo<Value>& args) {
		args.GetReturnValue().Set(Nan::New(canvas_imageSmoothingEnabled_get() == VG_TRUE));
	}


	void SetGlobalCompositeOperation(const Nan::FunctionCallbackInfo<Value>& args) {
		if(args.Length() != 1 || !args[0]->IsString()) {
			Nan::ThrowTypeError("wrong arg");
			return;
		}

		canvas_globalCompositeOperation(*Nan::Utf8String(args[0]));

	}

	void GetGlobalCompositeOperation(const Nan::FunctionCallbackInfo<Value>& args) {
		args.GetReturnValue().Set(Nan::New(canvas_globalCompositeOperation_get()).ToLocalChecked());
	}
	
	
	void GetMiterLimit(const Nan::FunctionCallbackInfo<Value>& args) {
		args.GetReturnValue().Set(Nan::New(canvas_miterLimit_get()));
	}

	void SetMiterLimit(const Nan::FunctionCallbackInfo<Value>& args) {
		if(!checkArgs(args, 1)) {
			Nan::ThrowTypeError("wrong arg");
			return;
		}

		canvas_miterLimit(args[0]->NumberValue());
	}
	
	
	void SetTextAlign(const Nan::FunctionCallbackInfo<Value>& args) {
		if(args.Length() != 1 || !args[0]->IsString()) {
			Nan::ThrowTypeError("wrong arg");
			return;
		}

		canvas_textAlign(*Nan::Utf8String(args[0]));

	}

	void GetTextAlign(const Nan::FunctionCallbackInfo<Value>& args) {
		args.GetReturnValue().Set(Nan::New(canvas_textAlign_get()).ToLocalChecked());
	}
	
	
	void SetTextBaseline(const Nan::FunctionCallbackInfo<Value>& args) {
		if(args.Length() != 1 || !args[0]->IsString()) {
			Nan::ThrowTypeError("wrong arg");
			return;
		}

		canvas_textBaseline(*Nan::Utf8String(args[0]));

	}

	void GetTextBaseline(const Nan::FunctionCallbackInfo<Value>& args) {
		args.GetReturnValue().Set(Nan::New(canvas_textBaseline_get()).ToLocalChecked());
	}
	
	
	void ResetTransform(const Nan::FunctionCallbackInfo<Value>& args) {
		canvas_resetTransform();
	}
	
	void Rotate(const Nan::FunctionCallbackInfo<Value>& args) {
		if(!checkArgs(args, 1)) {
			Nan::ThrowTypeError("wrong arg");
		}
		
		canvas_rotate(args[0]->NumberValue());
	}
	
	void Scale(const Nan::FunctionCallbackInfo<Value>& args) {
		if(!checkArgs(args, 2)) {
			Nan::ThrowTypeError("wrong arg");
		}
		
		canvas_scale(args[0]->NumberValue(), args[1]->NumberValue());
	}
	
	void Translate(const Nan::FunctionCallbackInfo<Value>& args) {
		if(!checkArgs(args, 2)) {
			Nan::ThrowTypeError("wrong arg");
		}
		
		canvas_translate(args[0]->NumberValue(), args[1]->NumberValue());
	}
	
	void Transform(const Nan::FunctionCallbackInfo<Value>& args) {
		if(!checkArgs(args, 6)) {
			Nan::ThrowTypeError("wrong arg");
		}
		
		canvas_transform(args[0]->NumberValue(), args[1]->NumberValue(), args[2]->NumberValue(), 
			args[3]->NumberValue(), args[4]->NumberValue(), args[5]->NumberValue());
	}
	
	void SetTransform(const Nan::FunctionCallbackInfo<Value>& args) {
		if(!checkArgs(args, 6)) {
			Nan::ThrowTypeError("wrong arg");
		}
		
		canvas_setTransform(args[0]->NumberValue(), args[1]->NumberValue(), args[2]->NumberValue(), 
			args[3]->NumberValue(), args[4]->NumberValue(), args[5]->NumberValue());
	}
	
	
	void MeasureText(const Nan::FunctionCallbackInfo<Value>& args) {
		if(args.Length() != 1 || !args[0]->IsString()) {
			Nan::ThrowTypeError("wrong arg");
			return;
		}
		
		canvas_measure_text_metrics_t metrics;
		canvas_measureText(&metrics, *Nan::Utf8String(args[0]));
		
		Local<Object> obj = Nan::New<Object>();
  	obj->Set(Nan::New("width").ToLocalChecked(), Nan::New(metrics.width));
		obj->Set(Nan::New("actualBoundingBoxLeft").ToLocalChecked(), Nan::New(metrics.actual_bounding_box_left));
		obj->Set(Nan::New("actualBoundingBoxRight").ToLocalChecked(), Nan::New(metrics.actual_bounding_box_right));
		obj->Set(Nan::New("fontBoundingBoxAscent").ToLocalChecked(), Nan::New(metrics.font_bounding_box_ascent));
		obj->Set(Nan::New("fontBoundingBoxDescent").ToLocalChecked(), Nan::New(metrics.font_bounding_box_descent));
		obj->Set(Nan::New("actualBoundingBoxAscent").ToLocalChecked(), Nan::New(metrics.actual_bounding_box_ascent));
		obj->Set(Nan::New("actualBoundingBoxDescent").ToLocalChecked(), Nan::New(metrics.actual_bounding_box_descent)); 
		obj->Set(Nan::New("emHeightAscent").ToLocalChecked(), Nan::New(metrics.em_height_ascent));
		obj->Set(Nan::New("emHeightDescent").ToLocalChecked(), Nan::New(metrics.em_height_descent));
		obj->Set(Nan::New("hangingBaseline").ToLocalChecked(), Nan::New(metrics.hanging_baseline));
		obj->Set(Nan::New("alphabeticBaseline").ToLocalChecked(), Nan::New(metrics.alphabetic_baseline));
		obj->Set(Nan::New("ideographicBaseline").ToLocalChecked(), Nan::New(metrics.ideographic_baseline));
		
		args.GetReturnValue().Set(obj);
		
	}
	
	void GetImageData(const Nan::FunctionCallbackInfo<Value>& args) {
		if(!checkArgs(args, 4)) {
			Nan::ThrowTypeError("wrongs args");
			return;
		}
		
		VGint x = args[0]->NumberValue();
		VGint y = args[1]->NumberValue();
		VGint w = args[2]->NumberValue();
		VGint h = args[3]->NumberValue();
		
		Local<ArrayBuffer> buffer = ArrayBuffer::New(args.GetIsolate(), w * h * 4);
		Local<Uint8ClampedArray> array = Uint8ClampedArray::New(buffer, 0, w * h * 4);
		
		vgReadPixels(buffer->GetContents().Data(), w * 4, VG_sABGR_8888, x, egl_get_height() - y - h, w, h);
		
		args.GetReturnValue().Set(array);
	}
	
	
	struct BlobData {
		uv_work_t work;
		std::string type;
		char *blob;
		char *src;
		size_t size;
		Nan::Callback callback;
		float encoder;
	};
	
	void BlobCreate(uv_work_t *work) {
		BlobData *data = static_cast<BlobData*>(work->data);
		
		data->blob = image_to_blob(data->src, data->type.c_str(), data->encoder, &data->size);

	}
	
	void BlobFinished(uv_work_t *work, int status) {
		Nan::HandleScope scope;
		BlobData *data = static_cast<BlobData*>(work->data);
		
		if(!data->blob) {
			Nan::ThrowError("failed to create blob");
			free(data->src);
			delete data;
			return;
		}
		
		Local<Value> buffer = Nan::NewBuffer(data->blob, data->size).ToLocalChecked();
		
		data->callback.Call(1, &buffer);
		
		free(data->src);
		delete data;
	}
	
	void ToBlob(const Nan::FunctionCallbackInfo<Value>& args) {
		if(args.Length() != 3 || !args[0]->IsFunction() || !args[1]->IsString() || !args[2]->IsNumber()) {
			Nan::ThrowTypeError("wrong args");
			return;
		}
		
		char *src = static_cast<char*>(malloc(egl_get_width() * egl_get_height() * 4));
		if(!src) {
			Nan::ThrowError("Failed to allocate memory");
			return;
		}
		
		vgReadPixels(src, egl_get_width() * 4, VG_sRGBX_8888, 0, 0, egl_get_width(), egl_get_height());
		
		std::string type = *Nan::Utf8String(args[1]);
		float encoder = args[2]->NumberValue();
		
		BlobData *data = new BlobData;
		data->work.data = data;
		data->callback.SetFunction(Local<Function>::Cast(args[0]));
		data->type = type;
		data->src = src;
		data->encoder = encoder;
		
		uv_queue_work(uv_default_loop(), &data->work, BlobCreate, BlobFinished);
		
	}
	
	void ToURL(const Nan::FunctionCallbackInfo<Value>& args) {
		if(args.Length() != 2 || !args[0]->IsString() || !args[1]->IsNumber()) {
			Nan::ThrowTypeError("wrong args");
			return;
		}
		
		char *src = static_cast<char*>(malloc(egl_get_width() * egl_get_height() * 4));
		if(!src) {
			Nan::ThrowError("Failed to allocate memory");
			return;
		}
		
		vgReadPixels(src, egl_get_width() * 4, VG_sRGBX_8888, 0, 0, egl_get_width(), egl_get_height());
		
		std::string type = *Nan::Utf8String(args[0]);
		float encoder = args[1]->NumberValue();
		
		char *base64 = image_to_data_url(src, type.c_str(), encoder);
		free(src);
		
		if(!base64) {
			Nan::ThrowError("failed to create data url");
			return;
		}
		
		args.GetReturnValue().Set(Nan::New(base64).ToLocalChecked());
	}

	void ModuleInit(Local<Object> exports) {
		exports->Set(Nan::New("init").ToLocalChecked(), Nan::New<FunctionTemplate>(Init)->GetFunction());
		exports->Set(Nan::New("swapBuffers").ToLocalChecked(), Nan::New<FunctionTemplate>(SwapBuffers)->GetFunction());
		exports->Set(Nan::New("cleanup").ToLocalChecked(), Nan::New<FunctionTemplate>(Cleanup)->GetFunction());

		exports->Set(Nan::New("fillRect").ToLocalChecked(), Nan::New<FunctionTemplate>(FillRect)->GetFunction());
		exports->Set(Nan::New("clearRect").ToLocalChecked(), Nan::New<FunctionTemplate>(ClearRect)->GetFunction());
		exports->Set(Nan::New("strokeRect").ToLocalChecked(), Nan::New<FunctionTemplate>(StrokeRect)->GetFunction());

		exports->Set(Nan::New("setStyle").ToLocalChecked(), Nan::New<FunctionTemplate>(SetStyle)->GetFunction());
		exports->Set(Nan::New("getStyle").ToLocalChecked(), Nan::New<FunctionTemplate>(GetFillStyle)->GetFunction());

		exports->Set(Nan::New("getScreenWidth").ToLocalChecked(), Nan::New<FunctionTemplate>(GetScreenWidth)->GetFunction());
		exports->Set(Nan::New("getScreenHeight").ToLocalChecked(), Nan::New<FunctionTemplate>(GetScreenHeight)->GetFunction());

		exports->Set(Nan::New("setLineWidth").ToLocalChecked(), Nan::New<FunctionTemplate>(SetLineWidth)->GetFunction());
		exports->Set(Nan::New("setLineCap").ToLocalChecked(), Nan::New<FunctionTemplate>(SetLineCap)->GetFunction());
		exports->Set(Nan::New("setLineJoin").ToLocalChecked(), Nan::New<FunctionTemplate>(SetLineJoin)->GetFunction());
		exports->Set(Nan::New("setLineDash").ToLocalChecked(), Nan::New<FunctionTemplate>(SetLineDash)->GetFunction());
		exports->Set(Nan::New("setLineDashOffset").ToLocalChecked(), Nan::New<FunctionTemplate>(SetLineDashOffset)->GetFunction());

		exports->Set(Nan::New("getLineWidth").ToLocalChecked(), Nan::New<FunctionTemplate>(GetLineWidth)->GetFunction());
		exports->Set(Nan::New("getLineCap").ToLocalChecked(), Nan::New<FunctionTemplate>(GetLineCap)->GetFunction());
		exports->Set(Nan::New("getLineJoin").ToLocalChecked(), Nan::New<FunctionTemplate>(GetLineJoin)->GetFunction());
		exports->Set(Nan::New("getLineDash").ToLocalChecked(), Nan::New<FunctionTemplate>(GetLineDash)->GetFunction());
		exports->Set(Nan::New("getLineDashOffset").ToLocalChecked(), Nan::New<FunctionTemplate>(GetLineDashOffset)->GetFunction());

		exports->Set(Nan::New("setGlobalAlpha").ToLocalChecked(), Nan::New<FunctionTemplate>(SetGlobalAlpha)->GetFunction());
		exports->Set(Nan::New("getGlobalAlpha").ToLocalChecked(), Nan::New<FunctionTemplate>(GetGlobalAlpha)->GetFunction());

		exports->Set(Nan::New("beginPath").ToLocalChecked(), Nan::New<FunctionTemplate>(BeginPath)->GetFunction());
		exports->Set(Nan::New("closePath").ToLocalChecked(), Nan::New<FunctionTemplate>(ClosePath)->GetFunction());
		exports->Set(Nan::New("moveTo").ToLocalChecked(), Nan::New<FunctionTemplate>(MoveTo)->GetFunction());
		exports->Set(Nan::New("lineTo").ToLocalChecked(), Nan::New<FunctionTemplate>(LineTo)->GetFunction());
		exports->Set(Nan::New("stroke").ToLocalChecked(), Nan::New<FunctionTemplate>(Stroke)->GetFunction());
		exports->Set(Nan::New("fill").ToLocalChecked(), Nan::New<FunctionTemplate>(Fill)->GetFunction());

		exports->Set(Nan::New("quadraticCurveTo").ToLocalChecked(), Nan::New<FunctionTemplate>(QuadraticCurveTo)->GetFunction());
		exports->Set(Nan::New("bezierCurveTo").ToLocalChecked(), Nan::New<FunctionTemplate>(BezierCurveTo)->GetFunction());
		exports->Set(Nan::New("arc").ToLocalChecked(), Nan::New<FunctionTemplate>(Arc)->GetFunction());
		exports->Set(Nan::New("rect").ToLocalChecked(), Nan::New<FunctionTemplate>(Rect)->GetFunction());

		exports->Set(Nan::New("clip").ToLocalChecked(), Nan::New<FunctionTemplate>(Clip)->GetFunction());

		exports->Set(Nan::New("save").ToLocalChecked(), Nan::New<FunctionTemplate>(Save)->GetFunction());
		exports->Set(Nan::New("restore").ToLocalChecked(), Nan::New<FunctionTemplate>(Restore)->GetFunction());
		
		exports->Set(Nan::New("setFont").ToLocalChecked(), Nan::New<FunctionTemplate>(SetFont)->GetFunction());
		exports->Set(Nan::New("loadFont").ToLocalChecked(), Nan::New<FunctionTemplate>(NewFont)->GetFunction());
		exports->Set(Nan::New("fillText").ToLocalChecked(), Nan::New<FunctionTemplate>(FillText)->GetFunction());
		exports->Set(Nan::New("strokeText").ToLocalChecked(), Nan::New<FunctionTemplate>(StrokeText)->GetFunction());
		exports->Set(Nan::New("measureText").ToLocalChecked(), Nan::New<FunctionTemplate>(MeasureText)->GetFunction());
		
		exports->Set(Nan::New("drawImage").ToLocalChecked(), Nan::New<FunctionTemplate>(DrawImage)->GetFunction());
		exports->Set(Nan::New("setImageSmoothing").ToLocalChecked(), Nan::New<FunctionTemplate>(SetImageSmoothing)->GetFunction());
		exports->Set(Nan::New("getImageSmoothing").ToLocalChecked(), Nan::New<FunctionTemplate>(GetImageSmoothing)->GetFunction());
		
		exports->Set(Nan::New("setGlobalCompositeOperation").ToLocalChecked(), Nan::New<FunctionTemplate>(SetGlobalCompositeOperation)->GetFunction());
		exports->Set(Nan::New("getGlobalCompositeOperation").ToLocalChecked(), Nan::New<FunctionTemplate>(GetGlobalCompositeOperation)->GetFunction());
		
		exports->Set(Nan::New("setMiterLimit").ToLocalChecked(), Nan::New<FunctionTemplate>(SetMiterLimit)->GetFunction());
		exports->Set(Nan::New("getMiterLimit").ToLocalChecked(), Nan::New<FunctionTemplate>(GetMiterLimit)->GetFunction());
		
		exports->Set(Nan::New("setTextAlign").ToLocalChecked(), Nan::New<FunctionTemplate>(SetTextAlign)->GetFunction());
		exports->Set(Nan::New("getTextAlign").ToLocalChecked(), Nan::New<FunctionTemplate>(GetTextAlign)->GetFunction());
		exports->Set(Nan::New("setTextBaseline").ToLocalChecked(), Nan::New<FunctionTemplate>(SetTextBaseline)->GetFunction());
		exports->Set(Nan::New("getTextBaseline").ToLocalChecked(), Nan::New<FunctionTemplate>(GetTextBaseline)->GetFunction());
		
		exports->Set(Nan::New("resetTransform").ToLocalChecked(), Nan::New<FunctionTemplate>(ResetTransform)->GetFunction());
		exports->Set(Nan::New("scale").ToLocalChecked(), Nan::New<FunctionTemplate>(Scale)->GetFunction());
		exports->Set(Nan::New("rotate").ToLocalChecked(), Nan::New<FunctionTemplate>(Rotate)->GetFunction());
		exports->Set(Nan::New("translate").ToLocalChecked(), Nan::New<FunctionTemplate>(Translate)->GetFunction());
		exports->Set(Nan::New("transform").ToLocalChecked(), Nan::New<FunctionTemplate>(Transform)->GetFunction());
		exports->Set(Nan::New("setTransform").ToLocalChecked(), Nan::New<FunctionTemplate>(SetTransform)->GetFunction());
		
		exports->Set(Nan::New("getImageData").ToLocalChecked(), Nan::New<FunctionTemplate>(GetImageData)->GetFunction());
		
		exports->Set(Nan::New("toBlob").ToLocalChecked(), Nan::New<FunctionTemplate>(ToBlob)->GetFunction());
		exports->Set(Nan::New("toDataURL").ToLocalChecked(), Nan::New<FunctionTemplate>(ToURL)->GetFunction());
		
		Gradient::Init(exports);
		Image::Init(exports);
		Pattern::Init(exports);

	}

}

NODE_MODULE(vgcanvas, vgcanvas::ModuleInit)
