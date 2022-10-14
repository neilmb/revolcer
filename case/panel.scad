include <common.scad>
SKIRT_DEPTH = 20;
TOP_THICKNESS = 2*PANEL_THICKNESS;

BUTTON_DIAMETER = 15.7;
ENCODER_DIAMETER = 7.05;
HEADPHONE_DIAMETER = 6 + 0.5; // 0.5mm padding

NEOPIXEL_LEFT_END = 30;
NEOPIXEL_TOP_EDGE = 56;
NEOPIXEL_WIDTH = 5;
NEOPIXEL_LENGTH = 100.5;
NEOPIXEL_RESISTOR_WIDTH = 3;
NEOPIXEL_RESISTOR_DEPTH = 1.4;
NEOPIXEL_RESISTOR_GAP = 0.5;

SLOT_CENTERLINE_X = 21;
SLOT_CENTERLINE_DOWN = 12;
USB_PORT_WIDTH = 8 + 0.5; // 0.5mm padding
USB_PORT_HEIGHT = 3 + 0.5; // 0.5mm padding
MICROSD_WIDTH = 11 + 0.5; // 0.5mm padding 
MICROSD_HEIGHT = 1 + 0.5; // 0.5mm padding
STUD_THICKNESS = SCREW_POSITION_OFFSET+SCREW_HEAD-1;

//FIXME
$fn=10;

use <lib/roundedcube.scad>

module structure() {
    // front panel
    difference() {
        translate([0, 0, -SKIRT_DEPTH]) roundedcube([WIDTH, HEIGHT, TOP_THICKNESS+SKIRT_DEPTH], radius=CORNER_RADIUS, apply_to="zmax");

        translate([PANEL_THICKNESS, PANEL_THICKNESS, -SKIRT_DEPTH]) cube([WIDTH-2*PANEL_THICKNESS, HEIGHT-2*PANEL_THICKNESS, SKIRT_DEPTH]);
    }
}

module button_hole(x, y) {
    // a hole for the button centered at x,y
    translate([x, y, 0])
    cylinder(h=3*TOP_THICKNESS, center=true, r=BUTTON_DIAMETER/2);
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
    translate([NEOPIXEL_LEFT_END, NEOPIXEL_TOP_EDGE - NEOPIXEL_WIDTH, -TOP_THICKNESS])
    cube([NEOPIXEL_LENGTH, NEOPIXEL_WIDTH, 3*TOP_THICKNESS]);

    // resistor notch
    translate([NEOPIXEL_LEFT_END, NEOPIXEL_TOP_EDGE - NEOPIXEL_WIDTH - NEOPIXEL_RESISTOR_GAP - NEOPIXEL_RESISTOR_WIDTH, 0])
    cube([NEOPIXEL_LENGTH, NEOPIXEL_RESISTOR_WIDTH, NEOPIXEL_RESISTOR_DEPTH]);
}

module neopixel_stud(x, y) {
    // mounting stud for neopixel
    translate([x, y, NEOPIXEL_RESISTOR_DEPTH - 2.5]) cylinder(h=4, r=0.8);
}

module neopixel_studs() {
    stud_center = NEOPIXEL_TOP_EDGE - NEOPIXEL_WIDTH - 2;
    post_offset = 12.5;
    neopixel_stud(NEOPIXEL_LEFT_END + post_offset, stud_center);
    neopixel_stud(NEOPIXEL_LEFT_END + post_offset * 3, stud_center);
    neopixel_stud(NEOPIXEL_LEFT_END + post_offset * 5, stud_center);
    neopixel_stud(NEOPIXEL_LEFT_END + post_offset * 7, stud_center);

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

module encoder() {
        translate([131, 24, 0]) cylinder(h=3*TOP_THICKNESS, center=true, r=ENCODER_DIAMETER/2);
}

module screw_stud(x, y) {
    translate([x, y, -SKIRT_DEPTH]) cylinder(h=SCREW_DEPTH, r=SCREW_SHAFT-0.1);
}

module case_studs() {
    difference() {
        translate([PANEL_THICKNESS, PANEL_THICKNESS, -SKIRT_DEPTH])
            cube([STUD_THICKNESS, STUD_THICKNESS, SKIRT_DEPTH]);
            
        screw_stud(SCREW1_X, SCREW1_Y);
    }
    
    difference() {
        translate([PANEL_THICKNESS, HEIGHT-PANEL_THICKNESS-STUD_THICKNESS, -SKIRT_DEPTH])
            cube([STUD_THICKNESS, STUD_THICKNESS, SKIRT_DEPTH]);
            
        screw_stud(SCREW2_X, SCREW2_Y);
    }

    difference() {
        translate([WIDTH-PANEL_THICKNESS-STUD_THICKNESS, PANEL_THICKNESS, -SKIRT_DEPTH])
            cube([STUD_THICKNESS, STUD_THICKNESS, SKIRT_DEPTH]);
            
        screw_stud(SCREW3_X, SCREW3_Y);
    }

    difference() {
        translate([WIDTH-PANEL_THICKNESS-STUD_THICKNESS, HEIGHT-PANEL_THICKNESS-STUD_THICKNESS, -SKIRT_DEPTH])
            cube([STUD_THICKNESS, STUD_THICKNESS, SKIRT_DEPTH]);
            
        screw_stud(SCREW4_X, SCREW4_Y);
    }
}

module case() {
    // drill holes
    difference() {
        structure();
        buttons();
        encoder();
        neopixels();
        headphones();
        slots();
    }
    case_studs();
    neopixel_studs();
}
//case();

//Test print
intersection() {
    case();
    translate([28, 45, 0]) cube([72,13,100]);
}


//TODO
// - Add labels
// - Add clips for teensy
// - Add neopixel mount

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
