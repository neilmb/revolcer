PANEL_THICKNESS = 7;
WIDTH = 150;
INNER_HEIGHT = 61;  // The length of the Teensy board
HEIGHT = INNER_HEIGHT + 2 * PANEL_THICKNESS;
SCREW_HEAD = 3.5;
SCREW_SHAFT = 1.5;
SCREW_RECESS_DEPTH = 4.3;
SCREW_POSITION_OFFSET = 6;
CORNER_RADIUS = 1;

SCREW1_X = SCREW_POSITION_OFFSET;        SCREW1_Y = SCREW_POSITION_OFFSET;
SCREW2_X = SCREW_POSITION_OFFSET;        SCREW2_Y = HEIGHT- SCREW_POSITION_OFFSET;
SCREW3_X = WIDTH-SCREW_POSITION_OFFSET;  SCREW3_Y = SCREW_POSITION_OFFSET;
SCREW4_X = WIDTH-SCREW_POSITION_OFFSET;  SCREW4_Y = HEIGHT-SCREW_POSITION_OFFSET;

$fn = 40;

include <lib/roundedcube.scad>

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
        roundedcube([WIDTH, HEIGHT, PANEL_THICKNESS], CORNER_RADIUS, "x");

        screw_hole(SCREW1_X, SCREW1_Y);
        screw_hole(SCREW2_X, SCREW2_Y);
        screw_hole(SCREW3_X, SCREW3_Y);
        screw_hole(SCREW4_X, SCREW4_Y);
    }
}

back_panel();
