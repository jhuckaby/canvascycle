## Old School Color Cycling with HTML5

Anyone remember [Color cycling](https://en.wikipedia.org/wiki/Color_cycling) from the 90s?  This was a technology often used in 8-bit video games of the era, to achieve interesting visual effects by cycling (shifting) the color palette.  Back then video cards could only render 256 colors at a time, so a palette of selected colors was used.  But the programmer could change this palette at will, and all the onscreen colors would instantly change to match.  It was fast, and took virtually no memory.  Thus began the era of color cycling.

Most games used the technique to animate water, fire or other environmental effects.  Unfortunately, more often than not this looked terrible, because the artist simply drew the scene once, picked some colors to be animated and set them to cycle.  While this technically qualified as "color cycling", it looked more like a bad acid trip.  For an example, just look at the water in [this game](https://www.youtube.com/watch?v=wfkEr3Bxoqg).

However, there was one graphic artist who took the technique to a whole new level, and produced absolutely breathtaking color cycling scenes.  [Mark J. Ferrari](https://www.markferrari.com/), who also illustrated all the original backgrounds for [LucasArts](https://en.wikipedia.org/wiki/Lucasfilm_Games) [Loom](https://en.wikipedia.org/wiki/Loom_%28video_game%29), and some for [The Secret of Monkey Island](https://en.wikipedia.org/wiki/The_Secret_of_Monkey_Island), invented his own unique ways of using color cycling for environmental effects that you really have to see to believe.  These include rain, snow, ocean waves, moving fog, clouds, smoke, waterfalls, streams, lakes, and more.  And all these effects are achieved without any layers or alpha channels -- just one single flat image with one 256 color palette.

Unfortunately the art of color cycling died out in the late 90s, giving way to newer technologies like 3D rendering and full 32-bit "true color" games.  However, 2D pixel graphics of old are making a comeback in recent years, with mobile devices and web games.  I thought now would be the time to reintroduce color cycling, using open web technologies like the [HTML5](https://en.wikipedia.org/wiki/HTML5) [Canvas element](https://en.wikipedia.org/wiki/Canvas_element).

This demo is an implementation of a full 8-bit color cycling engine, rendered into an HTML5 Canvas in real-time.  I am using 35 of Mark's original 640x480 pixel masterpieces which you can explore, and I added some ambient environmental soundtracks to match.  Please enjoy, and the source code is free for you to use in your own projects (download links at the bottom of the article).

![Screenshot](https://pixlcore.com/software/canvascycle/screenshot.png)

- **[Launch Demo With Sound](http://www.effectgames.com/demos/canvascycle/)**
- **[Launch Demo Without Sound](http://www.effectgames.com/demos/canvascycle/?sound=0)**

## Q & A with Mark J. Ferrari

Hey everyone!  Mark has generously donated some of his time to answer the most popular questions on his color cycling artwork.  Please read about it here: [Q & A with Mark J. Ferrari](http://www.effectgames.com/effect/article.psp.html/joe/Q_A_with_Mark_J_Ferrari).

## BlendShift Cycling

Those of you familiar with color cycling may notice something a little "different" about the palette animation in this engine.  Many years ago I had an idea to improve color cycling by "fading" colors into each other as they shifted, to produce many "in between" frames, while preserving the overall "speed" of the cycling effect.  This creates a much smoother appearance, and gives the illusion of more colors in the scene.  I call this technique **BlendShift Cycling**.  Someone may have invented this before me, but I've certainly never seen it used.

You can really see the effect this has if you slow down the cycling speed (Click "Show Options" and click on either &frac14; or &frac12;), then turn BlendShift off and on by clicking on the "Standard" and "Blend" cycling modes.  See the difference?  The colors "shift" positions in whole steps when using Standard mode, but fade smoothly into each other when using BlendShift.  If only I'd invented this trick 20 years ago when it really mattered!

## Optimization

In order to achieve fast frame rates in the browser, I had to get a little crazy in the engine implementation.  Rendering a 640x480 indexed image on a 32-bit RGB canvas means walking through and drawing 307,200 pixels per frame, in JavaScript.  That's a very big array to traverse, and some browsers just couldn't keep up.  To overcome this, I pre-process the images when they are first loaded, and grab the pixels that reference colors which are animated (i.e. are part of cycling sets in the palette).  Those pixel X/Y offsets are stored in a separate, smaller array, and thus only the pixels that change are refreshed onscreen.

The framerate is capped at 60 FPS.

## Amiga IFF / LBM Files

Mark's scenes are actually [Amiga IFF / ILBM](https://en.wikipedia.org/wiki/ILBM) files, originally created with [Deluxe Paint](https://en.wikipedia.org/wiki/Deluxe_Paint) in DOS.  Ah, those were the days!  So, to make this work, I had to write a converter program which parses the files and extracts the pixels, the palette colors, and all the cycling information, and writes it out as something JavaScript can understand.  The data is stored as [JSON](https://en.wikipedia.org/wiki/JSON) on disk, and delivered to the browser with gzip compression.  The data sent over the wire ends up being about 100K per scene, which isn't too bad (most of the soundtracks are larger than that, haha).  My converter script is written in C++, but included in the source package if you are interested.

**Update:** I also wrote a Node.js implementation of my LBM converter utility: [lbmtool](https://github.com/jhuckaby/lbmtool)

## Browser Support

The color cycling engine works in all modern browsers.

## Download Source

Here is the JavaScript and C++ source code to my color cycling engine.  I am releasing it under the [MIT License](https://github.com/jhuckaby/canvascycle/blob/main/LICENSE.md).  The package comes with one test LBM image, converted to JSON.  The actual artwork shown in the demo is copyright, and cannot be used.

https://github.com/jhuckaby/canvascycle/archive/refs/heads/main.zip
