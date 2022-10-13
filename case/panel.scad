PANEL_THICKNESS = 1.6;
SKIRT_DEPTH = 20;
WIDTH = 150;
INNER_HEIGHT = 61;  // The length of the Teensy board
HEIGHT = INNER_HEIGHT + 2 * PANEL_THICKNESS;
CORNER_RADIUS = 1;

BUTTON_DIAMETER = 15.7;
ENCODER_DIAMETER = 7.05;
HEADPHONE_DIAMETER = 6 + 0.5; // 0.5mm padding

NEOPIXEL_LEFT_END = 30;
NEOPIXEL_TOP_EDGE = 56;

SLOT_CENTERLINE_X = 15;
SLOT_CENTERLINE_DOWN = 10;
USB_PORT_WIDTH = 8 + 0.5; // 0.5mm padding
USB_PORT_HEIGHT = 3 + 0.5; // 0.5mm padding
MICROSD_WIDTH = 11 + 0.5; // 0.5mm padding 
MICROSD_HEIGHT = 1 + 0.5; // 0.5mm padding

$fn=40;

include <lib/roundedcube.scad>

module structure() {
    // front panel
    cube([WIDTH, HEIGHT, PANEL_THICKNESS]);

    // skirt
    translate([0, 0, -SKIRT_DEPTH]) roundedcube([WIDTH, PANEL_THICKNESS, SKIRT_DEPTH], CORNER_RADIUS, "zmax");
    translate([0, HEIGHT - PANEL_THICKNESS, -SKIRT_DEPTH]) roundedcube([WIDTH, PANEL_THICKNESS, SKIRT_DEPTH], CORNER_RADIUS, "zmax");
    translate([0, 0, -SKIRT_DEPTH]) roundedcube([PANEL_THICKNESS, HEIGHT, SKIRT_DEPTH], CORNER_RADIUS, "zmax");
    translate([WIDTH - PANEL_THICKNESS, 0, -SKIRT_DEPTH]) roundedcube([PANEL_THICKNESS, HEIGHT, SKIRT_DEPTH], CORNER_RADIUS, "zmax");
}

module button_hole(x, y) {
    // a hole for the button centered at x,y
    translate([x, y, 0])
    cylinder(h=3*PANEL_THICKNESS, center=true, r=BUTTON_DIAMETER/2);
}

module buttons() {
    // all the holes for the five buttons
    button_hole(45, 13);
    button_hole(45, 35);
    button_hole(68, 13);
    button_hole(68, 35);
    button_hole(91, 13);
}

module neopixels() {
    // neopixel openings
    translate([NEOPIXEL_LEFT_END, NEOPIXEL_TOP_EDGE - 5, -PANEL_THICKNESS])
    cube([102, 5, 3*PANEL_THICKNESS]);
}

module headphones() {
    translate([WIDTH - 15, 50, -SKIRT_DEPTH / 2])
    rotate([-90, 0, 0])
    cylinder(h=100, r=HEADPHONE_DIAMETER/2);
}

module slots() {
    // USB port on top edge
    translate([SLOT_CENTERLINE_X - USB_PORT_WIDTH/2, 50, -SLOT_CENTERLINE_DOWN - USB_PORT_HEIGHT/2])
    cube([USB_PORT_WIDTH, 100, USB_PORT_HEIGHT]);
    
    //MicroSD on bottom edge
    translate([SLOT_CENTERLINE_X - MICROSD_WIDTH/2, -50, -SLOT_CENTERLINE_DOWN - MICROSD_HEIGHT/2])
    cube([MICROSD_WIDTH, 100, MICROSD_HEIGHT]);
}

module case() {
    // drill holes
    difference() {
        structure();
        buttons();
        // encoder
        translate([131, 24, 0]) cylinder(h=3*PANEL_THICKNESS, center=true, r=ENCODER_DIAMETER/2);
        neopixels();
        headphones();
        slots();
    }
}
case();


//TODO
// - Add labels
// - Add back panel
// - Add clips for teensy
// - Add neopixel mount
// - Smooth edges
// - Add studs for screws and recessed 

// encoder button test
//intersection() {
//    case();
//    translate([120, 0, -500]) cube([30, 35, 1000]);
//}

// shave down the neopixel mount STL from Thingiverse
//color("green")
//difference() {
//    import("neopixel_case.stl");
//    translate([-100, -100, 1]) cube([200, 200, 35]);
//}
