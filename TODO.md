# Todo

- [x] color strings: https://developer.mozilla.org/en-US/docs/Web/CSS/color_value (javascript code)
    - [x] color names
    - [x] #-hexadecimal
    - [x] rgb()
    - [x] rgba()
    - [x] hsl()
    - [x] hsla()
- [x] Rename "`src/infoscreen.cc`" to "`src/vgcanvas.cc`"
- [x] gradients
- [x] @Gebatzens: patterns
- [x] shadows *Won't implement*
- [x] images
- [x] text
- [x] "`openSubPath`" in "`src/canvas.c`" *Not needed*
- [x] https://developer.mozilla.org/en-US/docs/Web/API/Path2D/Path2D *Unable to implement because paths are only stored in the VRAM. There are no plans to change that.*
- [x] Remove "`src/color.*`"
- [x] Adjust coordinates
- [x] `eprintf` for errors
- [x] coordinates for closePath? *No*
- [x] replace all `void`-return-types with `int`-return-types
- [x] remove anonymous structs
- [x] `miterLimit`: what values do effect what? OpenVG documentation does not inform well about that topic. *Ratio of the `lineWidth`*
- [x] set standard values at initialization
- [x] create font if it is firstly used *No, defaults to "no font".*
- [x] Replace `onecolor` with regular expressions and native code
- [x] `textBaseline`, `textAlign`
- [x] add properties for `fillText` and `strokeText`
    - [x] gradients
- [x] add dependencies for `#include`s
- [x] error handling for `[fill|stroke]Text()` when no font has been loaded *Text is simply not rendered (silent, change?)*
- [x] Use new paths for `[fill|stroke]Rect()`
- [x] `closePath` for `strokeText()`
- [x] @Gebatzens: `imageSmoothingEnabled` (also in `save` and `restore`)
- [x] remove unused variables and struct-typedefs
- [x] reduce log output to a minimum (only errors)
- [x] remove `egl_debug_print_matrices()` from `src/egl-util.c`
- [ ] Text rendering: Unicode / UTF-8 support

---

- [x] @Gebatzens: `canvas.toBlob()`
- [x] @Gebatzens: `canvas.toDataURL()`
- [x] @Gebatzens: `ctx.createImageData()`
- [x] @Gebatzens: `ctx.putImageData()`
- [x] @Gebatzens: `ctx.getImageData()`
- [x] @Gebatzens: How does patterns are affected by `ctx.fillText()`, `ctx.strokeText()`
- [x] @Gebatzens: `requestAnimationFrame()` (~~global~~ `canvas.requestAnimationFrame`)
- [x] @NIPE-SYSTEMS: `miterLimit` in `ctx.strokeText()`
- [x] @Gebatzens: image-util error handling (e.g. see `src/font-util.c`)
- [x] @Gebatzens: documenting the image code in the `README.md` (Developer Informations) (describe all differences to the *Canvas 2D API*)
- [x] @Gebatzens: commenting the image-util code (`ctx.drawImage()` and other API functions)
- [x] @Gebatzens: free VRAM on fatal (unrecoverable) errors (http://izs.me/v8-docs/classv8_1_1V8.html#ab386f81a6d58dcf481d00446e8d15c9e)
- [x] @NIPE-SYSTEMS: remove `Makefile`, remove "`bin`" from "`.gitignore`", remove "`src/home-infoscreen.c`"
