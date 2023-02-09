/* xml.c: stuff needed to create a valid gpx file. */

#define HEADER "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\" ?>"

/* Note that this boilerplate is incredibly touchy. For example, version MUST
be 1.1 (not 1.0) or basecamp, e.g., won't import it. Must be this version is
not the version of the creator? */

#define GPX_ATTRIBUTE "xmlns=\"http://www.topografix.com/GPX/1/1\" \
xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" \
xmlns:gpxx=\"http://www.garmin.com/xmlschemas/GpxExtensions/v3\" \
xmlns:gpxtpx=\"http://www.garmin.com/xmlschemas/TrackPointExtension/v1\" \
creator=\"gpxpoly\" version=\"1.1\" \
xsi:schemaLocation=\"http://www.topografix.com/GPX/1/1 \
http://www.topografix.com/GPX/1/1/gpx.xsd \
http://www.garmin.com/xmlschemas/GpxExtensions/v3 \
http://www.garmin.com/xmlschemas/GpxExtensionsv3.xsd \
http://www.garmin.com/xmlschemas/TrackPointExtension/v1 \
http://www.garmin.com/xmlschemas/TrackPointExtensionv1.xsd\""
#define METADATA "<metadata>\
    <link href=\"http://www.garmin.com\">\
      <text>Garmin International</text>\
    </link>\
    <time>2009-10-17T22:58:43Z</time>\
</metadata>"

#define TEST_NAME "Camp to DEC Lot Direct"
