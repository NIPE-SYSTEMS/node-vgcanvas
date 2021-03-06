var vgcanvas = require('../lib/canvas');
var tests = [require('./colorPaint'), require('./alpha'), require('./gradient'), require('./image'), require('./text')];
require('keypress')(process.stdin);

var canvas = new vgcanvas.Canvas();
var ctx = canvas.getContext('2d');

ctx.loadFont('./test/Lato-Regular.ttf', 'font');
var w = canvas.width;
var h = canvas.height;
var current = 0;

var img = new vgcanvas.Image();
img.onload = function() {
	console.log("IMAGE LOADED " + JSON.stringify(img));
}

img.onerror = function(error) {
	console.error(error);
}

img.src = 'test/test2.png';
ctx.testImg = img;

ctx.testImg2 = new vgcanvas.Image();
ctx.testImg2.src = 'test/test.png';

function test() {
	if(current == tests.length) {
		console.log('All tests finished');
		ctx.cleanup();
		return process.stdin.pause();
	}
	
	canvas.requestAnimationFrame(function() {
		ctx.clearRect(0, 0, w, h);
		ctx.font = '20px font';
		var str = 'Test ' + current + ': ' + tests[current].name;
		console.log(str);
		ctx.fillStyle = '#000';
		ctx.strokeStyle = '#000';
		ctx.fillText(str, 10, 30);
		tests[current].test(ctx, w, h);
		current++;

	});
}

process.stdin.on('keypress', function(ch, key) {
	if(key && key.name == 'return') {
		test();
	} else if(key && key.name == 'c' && key.ctrl) {
		ctx.cleanup();
		process.stdin.pause();
	}
});

test();
process.stdin.setRawMode(true);
process.stdin.resume();
