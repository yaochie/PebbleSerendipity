var range;
var coordinates;
var key = "AIzaSyDBqEPiSyCPvIKrVneA95F-yOj3ib-d02I";

function setRange(newRange) {
    range = newRange;
}

function navigate(destination) {
    var req = new XMLHttpRequest();
    req.open('GET', "https://maps.googleapis.com/maps/api/directions/json?" +
             "origin=" + coordinates.latitude + "," + coordinates.longitude +
             "&destination=" + destination.lat + "," + destination.long +
             "&key=" + key + "&mode=walking");
    req.onload = function(e) {
        if (req.readyState == req.DONE) {
            if (req.status == 200) {                
                var response = JSON.parse(req.responseText);
                if (response.status == "OK") {
                    var route = response.routes[0];
                    var warnings = route.warnings;
                    console.log("warnings: " + JSON.stringify(warnings));
                    var steps = route.legs[0].steps;
                    var endAddress = route.legs[0].end_address;
                    console.log(endAddress);
                    for (var i=0; i<steps.length; i++) {
                        console.log(steps[i].html_instructions);
                    }
                } else if (response.status == "ZERO_RESULTS") {
                    //find a new location
                }
            }
        }
    };
    req.send(null);
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
                Pebble.sendAppMessage({
                    'COORDS_LAT': String(chosenPhoto.latitude),
                    'COORDS_LONG': String(chosenPhoto.longitude)
                });
                var endpoint = {'lat': chosenPhoto.latitude, 'long': chosenPhoto.longitude};
                navigate(endpoint);
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
});

Pebble.addEventListener("appmessage", function(e) {
    //handle app message
    console.log("Got message: " + JSON.stringify(e));
    navigator.geolocation.getCurrentPosition(locationSuccess, locationError, locationOptions);
});