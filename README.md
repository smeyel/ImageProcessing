@mainpage

ImageProcessing
===============

2D Image processing submodule of the SMEyeL system
Project website: https://www.aut.bme.hu/Pages/ResearchEn/SMEyeL/Overview

libTwoColorCircleMarker
-----------------------

This library includes everything needed to locate a TwoColorCircleMarker in a given image. To use it, do the followings:
- Create a class derived from TwoColorCircleMarker::DetectionResultExporterBase which will be notified every time a marker is found.
- Instantiate TwoColorCircleMarker::MarkerCC2Tracker
- Use its setResultExporter to set the target of marker exporting created above.
- Call ist init() function to initialize it. Define the configuration ini files name here and all further settings will be read from there. (Example for that is provided in the repository with the executable projects using this library, like M1 and M2.)

After capturing an image frame, use the tracker's processFrame() function to find the marker.

Example code:

	DetectionCollector detectionCollector;
	TwoColorCircleMarker::MarkerCC2Tracker *tracker;
	tracker = new TwoColorCircleMarker::MarkerCC2Tracker();
	tracker->setResultExporter(&detectionCollector);
	tracker->init(configfilename,true,dsize.width,dsize.height);

	tracker->processFrame(frame,cameraID,frameIdx);

If you want to see the internal operation subresults of the marker search, activate visualization functions in the init file, and use the internally managed image tracker->visColorCodeFrame to see the output of the FastColorFilter as well.

If you activate verbosing functions, the tracker automatically sets the input frame as a verbose output image, so everything is drawn on the input image.

TODO: If you want verbose data to appear in another image, set tracker->twoColorLocator.verboseImage and tracker->markerCC2Locator.verboseImage accordingly. Currently, that will be overriden by tracker->processFrame().

If you need to finetune the color recognition in FastColorFilter, use the
tracker->fastColorFilter.setLutItem() function to assign a given color code to a given RGB value. (Have a look at that function to see the color quantizing step introduced to limit the size of the look-up-table FastColorFilter::RgbLut.)
If you need interactive color settings, you can use this function in an onClick event handler.
