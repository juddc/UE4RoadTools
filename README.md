UE4RoadTools
============
Tools for creating and editing roads and paths for Unreal Engine 4.


Current Status
--------------
Right now, you can drag in a Road actor, manipulate the spline, and add segments to the road.
It generally works pretty well.

The biggest current issue is the UI. You need to maintain the segments array manually in the editor,
and create the correct number of segments. Drilling down into the segments array to change settings
like the number of subdivisions or the spline scale is mildly annoying.

Any setting that can be pulled directly from the spline data (spline point position/rotation/scale) works
great.


Installing
----------
The easiest way to install the plugin is placing the contents of the repository in your project's Plugins
folder.

```
<YourProject>/Plugins/RoadTools/RoadTools.uplugin
<YourProject>/YourProject.uproject
```

You'll need to right-click your .uproject file, click "Generate Visual Studio project files", and rebuild
your project after putting the files in place.


Creating and Manipulating Road Actors
-------------------------------------
To create a Road actor, simply drag one in from the Class Browser or Modes panel. You will see a single
spline with two points. To make it show a mesh, you'll need to add a new Segment. In the details pane,
under Road, hit the + button next to Segments to add a new Segment. Now you should see a mesh. Click
one of the spline points, you should be able to move it around with the translate tool.

To add additional spline points, select one of the points, make sure you're on the Translate tool, and
alt-drag it out. You'll need to create an additional segment to see a mesh covering that part.

Segments hold data for the meshes between each pair of spline points. You can edit the spline itself using
the translate/rotate/scale tools, plus tweak each segment by changing the settings for each one.
