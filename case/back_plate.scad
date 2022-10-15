// Back plate for revolcer project box
// Uses M3x8 screws with washers and lockwashers

include <common.scad>
use <lib/roundedcube.scad>

PLATE_THICKNESS = 6;
SCREW_RECESS_DEPTH = 3.3;

module screw_hole(x, y) {
    // a hole for a recessed screw at x, y
    translate([x, y, 0]) {
        cylinder(h=100, r=SCREW_SHAFT);
        cylinder(h=SCREW_RECESS_DEPTH, r=SCREW_HEAD);
    }
}

module back_panel() {
    // holes
    difference() {
        // base plate
        roundedcube(size=[WIDTH, HEIGHT, PLATE_THICKNESS+CORNER_RADIUS], radius=CORNER_RADIUS);
        translate([0, 0, PLATE_THICKNESS]) cube([WIDTH, HEIGHT, 100]);

        screw_hole(SCREW1_X, SCREW1_Y);
        screw_hole(SCREW2_X, SCREW2_Y);
        screw_hole(SCREW3_X, SCREW3_Y);
        screw_hole(SCREW4_X, SCREW4_Y);
    }

    // CPU Support Stud
    translate([SLOT_CENTERLINE_X - (7/2), PANEL_THICKNESS + 25, PLATE_THICKNESS]) cube([7, 7, SKIRT_DEPTH - SLOT_TOP_DOWN - CPU_CHIP_THICKNESS]);

}

//back_panel();

// TEST code
intersection() {
    back_panel();
    translate([0, 0, -100]) cube([30,70,1000]);
}