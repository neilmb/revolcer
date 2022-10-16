include <common.scad>
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

// uncomment for faster rendering 
//$fn=10;

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
    translate([x, y, NEOPIXEL_RESISTOR_DEPTH - 5]) cylinder(h=5, r=0.8);
}

module neopixel_studs() {
    stud_center = NEOPIXEL_TOP_EDGE - NEOPIXEL_WIDTH - 2;
    post_offset = 12.6;
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
    translate([PANEL_THICKNESS + LID_STUD_THICKNESS + 5, 50, -SLOT_TOP_DOWN - USB_PORT_HEIGHT])
    cube([USB_PORT_WIDTH, 100, USB_PORT_HEIGHT]);
    
    //MicroSD on bottom edge
    translate([PANEL_THICKNESS + LID_STUD_THICKNESS + 3, -50, -SLOT_TOP_DOWN - MICROSD_HEIGHT])
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
            cube([LID_STUD_THICKNESS, LID_STUD_THICKNESS, SKIRT_DEPTH]);
            
        screw_stud(SCREW1_X, SCREW1_Y);
    }
    
    difference() {
        translate([PANEL_THICKNESS, HEIGHT - PANEL_THICKNESS -  LID_STUD_THICKNESS, -SKIRT_DEPTH])
            cube([LID_STUD_THICKNESS, LID_STUD_THICKNESS, SKIRT_DEPTH]);
            
        screw_stud(SCREW2_X, SCREW2_Y);
    }

    difference() {
        translate([WIDTH - PANEL_THICKNESS - LID_STUD_THICKNESS, PANEL_THICKNESS, -SKIRT_DEPTH])
            cube([LID_STUD_THICKNESS, LID_STUD_THICKNESS, SKIRT_DEPTH]);
            
        screw_stud(SCREW3_X, SCREW3_Y);
    }

    difference() {
        translate([WIDTH - PANEL_THICKNESS - LID_STUD_THICKNESS, HEIGHT - PANEL_THICKNESS - LID_STUD_THICKNESS, -SKIRT_DEPTH])
            cube([LID_STUD_THICKNESS, LID_STUD_THICKNESS, SKIRT_DEPTH]);
            
        screw_stud(SCREW4_X, SCREW4_Y);
    }
}

module cpu_support_posts() {
    gap = SLOT_TOP_DOWN - CPU_BOARD_THICKNESS;
    centerline = PANEL_THICKNESS + LID_STUD_THICKNESS + 5 + (USB_PORT_WIDTH / 2);
    translate([SLOT_CENTERLINE_X - (3.5/2), PANEL_THICKNESS + 8, -gap]) cube([3.5, 3.5, gap]);
    translate([SLOT_CENTERLINE_X - (3/2), PANEL_THICKNESS + 36, -gap]) cube([3, 3, gap]);
}

module label() {
    translate([14, 4, TOP_THICKNESS-0.6]) 
        rotate([0, 0, 90])
            linear_extrude(2)
                text("ReVolcer", size=10);
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
        label();
    }
    case_studs();
    neopixel_studs();
    cpu_support_posts();
}
case();

//Test neopixel print
//intersection() {
//    case();
//    translate([28, 45, -20]) cube([105,13,100]);
//}

// End cut
//intersection() {
//   case();
//    translate([0, 0, -50]) cube([35,100,100]);
//}

