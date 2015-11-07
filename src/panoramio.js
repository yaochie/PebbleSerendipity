var range;
var coordinates;

function setRange(newRange) {
    range = newRange;
}

function fetchPanoLocation() {    
    var req = new XMLHttpRequest();
    var minx = coordinates.longitude - range;
    var maxx = coordinates.longitude + range;
    var miny = coordinates.latitude - range;
    var maxy = coordinates.latitude + range;
    var numPhotos = 100;
    req.open('GET', "http://www.panoramio.com/map/get_panoramas.php?set=public&from=0" +
             "&to=" + numPhotos + "&minx=" + minx + "&miny=" + miny + "&maxx=" + maxx +
             "&maxy=" + maxy + "&size=medium");
    req.onload = function(e) {
        if (req.readyState == req.DONE) {
            if (req.status == 200) {
                console.log("received: " + req.responseText);
                
                var response = JSON.parse(req.responseText);
                var chosenPhoto = response.photos[Math.floor(Math.random() * numPhotos)];
                console.log(response.count);
                console.log("lat: " + chosenPhoto.latitude + " long: " + chosenPhoto.longitude);
                console.log(String(chosenPhoto.latitude));
                Pebble.sendAppMessage({
                    'COORDS_LAT': String(chosenPhoto.latitude),
                    'COORDS_LONG': String(chosenPhoto.longitude)
                });
            } else {
                console.log("Error with request: " + req.status);
            }
        }
    };
    req.send(null);
}

function locationSuccess(pos) {
    coordinates = pos.coords;
    fetchPanoLocation();
}

function locationError(err) {
    console.warn("Location error (" + err.code + "): " + err.message);
    //send blank result?
}

var locationOptions = {'timeout': 10000, 'maximumAge': 60000};

Pebble.addEventListener("ready", function(e) {
    console.log('Javascript app ready and running!');
    setRange(0.5);
    navigator.geolocation.getCurrentPosition(locationSuccess, locationError, locationOptions);
});

Pebble.addEventListener("appmessage", function(e) {
    //handle app message
    console.log("Got message: " + JSON.stringify(e));
    navigator.geolocation.getCurrentPosition(locationSuccess, locationError, locationOptions);
});