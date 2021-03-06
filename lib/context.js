var vgcanvas = require('../build/Release/vgcanvas');
var color = require('./color').decode;
var ImageData = require('./imageData');

var states = [];
var ctxUsed = false;

var VGContext = function(canvas) {
	if(ctxUsed) {
		throw new Error('Failed to initialize context: Only one context can be initialized at the same time');
	}
	
	var self = this;
	this.canvas = canvas;

	vgcanvas.init();

	function cleanup() {
		vgcanvas.cleanup();
		process.exit(0);
	}

	process.on('SIGTERM', cleanup);
	process.on('SIGINT', cleanup);

	self.fillStyleValue = '#000';
	self.strokeStyleValue = '#000';
	self.fontValue = undefined;

	Object.defineProperty(this, "fillStyle", {
		set: function(value) {
			self.setStyle(false, value);
			self.fillStyleValue = value;
		},
		get: function() {
			return self.fillStyleValue;
		}
	});

	Object.defineProperty(this, "strokeStyle", {
		set: function(value) {
			self.setStyle(true, value);
			self.strokeStyleValue = value;
		},
		get: function() {
			return self.strokeStyleValue;
		}
	});

	Object.defineProperty(this, "lineWidth", {
		set: vgcanvas.setLineWidth,
		get: vgcanvas.getLineWidth
	});

	Object.defineProperty(this, "lineCap", {
		set: vgcanvas.setLineCap,
		get: vgcanvas.getLineCap
	});

	Object.defineProperty(this, "lineJoin", {
		set: vgcanvas.setLineJoin,
		get: vgcanvas.getLineJoin
	});

	Object.defineProperty(this, "lineDashOffset", {
		set: vgcanvas.setLineDashOffset,
		get: vgcanvas.getLineDashOffset
	});

	Object.defineProperty(this, "globalAlpha", {
		set: vgcanvas.setGlobalAlpha,
		get: vgcanvas.getGlobalAlpha
	});

	Object.defineProperty(this, "font", {
		set: function(font) {
			var parts = font.split(' ');
			var size = parseInt(parts[0].substring(0, parts[0].length - 2));

			self.fontValue = font;
			vgcanvas.setFont(size, font.substring(parts[0].length + 1));
		},
		get: function() {
			return self.fontValue;
		}
	});
	
	Object.defineProperty(this, 'imageSmootingEnabled', {
		set: function(value) {
			vgcanvas.setImageSmoothing(value);
		},
		get: function() {
			return vgcanvas.getImageSmoothing();
		}
	});
	
	Object.defineProperty(this, 'globalCompositeOperation', {
		set: function(value) {
			vgcanvas.setGlobalCompositeOperation(value);
		},
		get: function() {
			return vgcanvas.getGlobalCompositeOperation();
		}
	});
	
	Object.defineProperty(this, 'miterLimit', {
		set: function(value) {
			vgcanvas.setMiterLimit(value);
		},
		get: function() {
			return vgcanvas.getMiterLimit();
		}
	});
	
	Object.defineProperty(this, 'textAlign', {
		set: function(value) {
			vgcanvas.setTextAlign(value);
		},
		get: function() {
			return vgcanvas.getTextAlign();
		}
	});
	
	Object.defineProperty(this, 'textBaseline', {
		set: function(value) {
			vgcanvas.setTextBaseline(value);
		},
		get: function() {
			return vgcanvas.getTextBaseline();
		}
	});
	
	ctxUsed = true;
	
};

vgcanvas.Gradient.prototype.addColorStop = function(pos, c) {
	c = color(c);
	this.addColorStopRGBA(pos, c[0], c[1], c[2], c[3]);
};

VGContext.prototype.createLinearGradient = function(x1, y1, x2, y2) {
	return new vgcanvas.Gradient('linear', x1, y1, x2, y2);
};

VGContext.prototype.createRadialGradient = function(cx, cy, r, fx, fy, fr) {
	return new vgcanvas.Gradient('radial', cx, cy, r, fx, fy);
};

VGContext.prototype.createPattern = function(image, repetition) {
	return new vgcanvas.Pattern(image, repetition);
};

VGContext.prototype.setStyle = function(type, obj) {
	if(typeof obj === 'string' || obj instanceof String) {
		obj = color(obj);
		vgcanvas.setStyle(type, [obj[0], obj[1], obj[2], obj[3]]);
	} else {
		vgcanvas.setStyle(type, obj);
	}
};

VGContext.prototype.setLineWidth = vgcanvas.setLineWidth;
VGContext.prototype.setLineCap = vgcanvas.setLineCap;
VGContext.prototype.setLineJoin = vgcanvas.setLineJoin;
VGContext.prototype.setLineDash = function(data) {
	if(data.length % 2 != 0)
		data = data.concat(data);

	this.lineDash = data;

	vgcanvas.setLineDash(data);
}
VGContext.prototype.setLineDashOffset = vgcanvas.setLineDashOffset;
VGContext.prototype.getLineDash = vgcanvas.getLineDash;

VGContext.prototype.fillRect = vgcanvas.fillRect;
VGContext.prototype.clearRect = vgcanvas.clearRect;
VGContext.prototype.strokeRect = vgcanvas.strokeRect;

VGContext.prototype.beginPath = vgcanvas.beginPath;
VGContext.prototype.closePath = vgcanvas.closePath;
VGContext.prototype.lineTo = vgcanvas.lineTo;
VGContext.prototype.moveTo = vgcanvas.moveTo;
VGContext.prototype.fill = vgcanvas.fill;
VGContext.prototype.stroke = vgcanvas.stroke;

VGContext.prototype.quadraticCurveTo = vgcanvas.quadraticCurveTo;
VGContext.prototype.bezierCurveTo = vgcanvas.bezierCurveTo;
VGContext.prototype.arc = vgcanvas.arc;
VGContext.prototype.rect = vgcanvas.rect;

VGContext.prototype.loadFont = vgcanvas.loadFont;
VGContext.prototype.setFont = vgcanvas.setFont;
VGContext.prototype.fillText = vgcanvas.fillText;
VGContext.prototype.strokeText = vgcanvas.strokeText;
VGContext.prototype.measureText = vgcanvas.measureText;

VGContext.prototype.resetTransform = vgcanvas.resetTransform;
VGContext.prototype.scale = vgcanvas.scale;
VGContext.prototype.rotate = vgcanvas.rotate;
VGContext.prototype.translate = vgcanvas.translate;
VGContext.prototype.setTransform = vgcanvas.setTransform;
VGContext.prototype.transform = vgcanvas.transform;

VGContext.prototype.drawImage = function(image, dx, dy, dw, dh, sx, sy, sw, sh) {
		
	if(dh === undefined || dw === undefined) {
		dw = image.width;
		dh = image.height;
	}
	
	if(sx === undefined || sy === undefined || sw === undefined || sh === undefined) {
		sx = 0;
		sy = 0;
		sw = image.width;
		sh = image.height;
	} else {
		// Swap destination and source
		return vgcanvas.drawImage(image, sx, sy, sw, sh, dx, dy, dw, dh);
	}
	
	vgcanvas.drawImage(image, dx, dy, dw, dh, sx, sy, sw, sh);
};

VGContext.prototype.getScreenWidth = vgcanvas.getScreenWidth;
VGContext.prototype.getScreenHeight = vgcanvas.getScreenHeight;

VGContext.prototype.clip = vgcanvas.clip;
VGContext.prototype.save = function() {
	states.push({ stroke: this.strokeStyleValue, fill: this.fillStyleValue, font: this.fontValue });
	return vgcanvas.save();
}
VGContext.prototype.restore = function() {
	if(states.length == 0) {
		throw new TypeError("states.length is 0");
	}
	var state = states.pop();
	this.strokeStyleValue = state.stroke;
	this.fillStyleValue = state.fill;
	this.fontValue = state.font;

	return vgcanvas.restore();
};

VGContext.prototype.getImageData = function(sx, sy, sw, sh) {
	var data = vgcanvas.getImageData(sx, sy, sw, sh);
	return new ImageData(data, sw, sh);
};

VGContext.prototype.putImageData = function(data, sx, sy, dx, dy, dw, dh) {
	if(dx === undefined || dy === undefined || dw === undefined || dh === undefined) {
		dx = 0;
		dy = 0;
		dw = data.width;
		dh = data.height;
	}
	
	//??
	sx += dx;
	sy += dy;
	
	this.drawImage(data._image, sx, sy, dw, dh);
};

VGContext.prototype.createImageData = function(width, height) {
	if(width instanceof ImageData) {
		return new ImageData(width.width, width.height);
	} else {
		if(width === 0 || height === 0) {
			throw new Error('width or height is zero');
		}
		
		return new ImageData(width, height);
	}
};

VGContext.prototype.toBlob = vgcanvas.toBlob;
VGContext.prototype.toDataURL = vgcanvas.toDataURL;

VGContext.prototype.swapBuffers = vgcanvas.swapBuffers;
VGContext.prototype.cleanup = function() {
	vgcanvas.cleanup();
	ctxUsed = false;
	/*if(gc) {
		gc();
	}*/
};

VGContext.prototype._loop = function() {
	var swap = false;
	var time = process.hrtime();
	time = time[0] * 1e3 + time[1] / 1e6;
	var funcs = this.canvas.funcs;
	this.canvas.funcs = {};
	
	for(var key in funcs) {
		swap = true;
		funcs[key](time);
	}
	
	if(swap) {
		this.swapBuffers();
	}

};

module.exports = VGContext;
