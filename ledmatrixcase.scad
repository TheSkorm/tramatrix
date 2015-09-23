// laser cut case for exp8266 and 8x8 LED matrix

$fn=20;
// all units in mm

//assumes headers are on the x axis and leds are facing positive z

// prefernces 
corner_radius=5;
layer_height=3;
layer_gap=0.2; // gap between layers

//case size
width=120;
height=75;
case_screw_radius=3;
case_screw_spacing=2; // distance from edge to put hole
case_depth=35;

//matrix size and mount locations
matrix_screw_radius=3;
//matrix_width=70;
//matrix_height=60;
matrix_offset=0; // this is the distance to move the matrix off center
// this assumes that mount holes are the same distance from the center.
matrix_screw_x=30;
matrix_screw_y=25;

// led display size and mount location (this is the actual led matrix, not just the board)
led_matrix_width=50;
led_matrix_height=50;
//asummed to be in the center of y
//x offset additional to matrix offset
led_matrix_offset=30; // eg how offcenter is it

//number of layers
num_layers = ceil(case_depth/(layer_gap+layer_height));

3d();
//2d();

module 3d(){
for (i =[0 : num_layers])
{
if (i==0) color("red") matrix_holes() case_holes() layer(); //bottom
else if (i==1)  color("red") translate([0,0,i*(layer_gap+layer_height)]) matrix_holes() case_holes() standoffs() side(); // standoffs
else if (i==num_layers)  color("white") translate([0,0,i*(layer_gap+layer_height)]) led_hole() case_holes() layer(); //top
else  color("black") translate([0,0,i*(layer_gap+layer_height)]) case_holes() side(); // the rest
}
}

module 2d(){
for (i =[0 : num_layers])
{
if (i==0)   projection(cut = true) matrix_holes() case_holes() layer(); //bottom
else if (i==1) translate ([0,i*(height*1.05),0]) projection(cut = true) matrix_holes() case_holes() standoffs() side(); // standoffs
else if (i==num_layers) translate ([0,i*(height*1.05),0]) projection(cut = true) led_hole() case_holes() layer(); //top
else translate ([0,i*(height*1.05),0]) projection(cut = true) case_holes() side(); // the rest
}
}

// modules and functions

// makes a layer

module layer() {
hull(){
translate([(width-corner_radius*2)/2,(height-corner_radius*2)/2,0]) cylinder(r=corner_radius,h=layer_height,center=true);
translate([(width-corner_radius*2)/2,(-height+corner_radius*2)/2,0]) cylinder(r=corner_radius,h=layer_height,center=true);
translate([(-width+corner_radius*2)/2,(height-corner_radius*2)/2,0]) cylinder(r=corner_radius,h=layer_height,center=true);
translate([(-width+corner_radius*2)/2,(-height+corner_radius*2)/2,0]) cylinder(r=corner_radius,h=layer_height,center=true);
}
}

module case_holes(){
screwx = (width/2)-case_screw_radius-case_screw_spacing;
screwy = (height/2)-case_screw_radius-case_screw_spacing;
difference(){
children();
translate ([screwx,screwy]) cylinder(d=case_screw_radius,h=layer_height, center=true);
translate ([screwx,-screwy]) cylinder(d=case_screw_radius,h=layer_height, center=true);
translate ([-screwx,screwy]) cylinder(d=case_screw_radius,h=layer_height, center=true);
translate ([-screwx,-screwy]) cylinder(d=case_screw_radius,h=layer_height, center=true);
}
}

module matrix_holes(){
difference(){
children();
translate ([matrix_screw_x+matrix_offset,matrix_screw_y]) cylinder(d=matrix_screw_radius,h=layer_height, center=true);
translate ([matrix_screw_x+matrix_offset,-matrix_screw_y]) cylinder(d=matrix_screw_radius,h=layer_height, center=true);
translate ([-matrix_screw_x+matrix_offset,matrix_screw_y]) cylinder(d=matrix_screw_radius,h=layer_height, center=true);
translate ([-matrix_screw_x+matrix_offset,-matrix_screw_y]) cylinder(d=matrix_screw_radius,h=layer_height, center=true);
}
}

module side(){
inner_width=width-(corner_radius*2)-case_screw_spacing;
inner_height=height-(corner_radius*2)-case_screw_spacing;
difference(){
layer();
hull(){
translate([(inner_width-corner_radius*2)/2,(inner_height-corner_radius*2)/2,0]) cylinder(r=corner_radius,h=layer_height,center=true);
translate([(inner_width-corner_radius*2)/2,(-inner_height+corner_radius*2)/2,0]) cylinder(r=corner_radius,h=layer_height,center=true);
translate([(-inner_width+corner_radius*2)/2,(inner_height-corner_radius*2)/2,0]) cylinder(r=corner_radius,h=layer_height,center=true);
translate([(-inner_width+corner_radius*2)/2,(-inner_height+corner_radius*2)/2,0]) cylinder(r=corner_radius,h=layer_height,center=true);
}
}
}

module standoffs(){
children();
hullheight = (height/2)-(matrix_screw_radius/2+case_screw_spacing*2);

hull(){
translate ([matrix_screw_x+matrix_offset,hullheight]) cylinder(d=matrix_screw_radius+case_screw_spacing*2,h=layer_height, center=true);

translate ([matrix_screw_x+matrix_offset,matrix_screw_y]) cylinder(d=matrix_screw_radius+case_screw_spacing*2,h=layer_height, center=true);
}

hull(){
translate ([matrix_screw_x+matrix_offset,-hullheight]) cylinder(d=matrix_screw_radius+case_screw_spacing*2,h=layer_height, center=true);

translate ([matrix_screw_x+matrix_offset,-matrix_screw_y]) cylinder(d=matrix_screw_radius+case_screw_spacing*2,h=layer_height, center=true);
}

hull(){
translate ([-matrix_screw_x+matrix_offset,hullheight]) cylinder(d=matrix_screw_radius+case_screw_spacing*2,h=layer_height, center=true);

translate ([-matrix_screw_x+matrix_offset,matrix_screw_y]) cylinder(d=matrix_screw_radius+case_screw_spacing*2,h=layer_height, center=true);
}

hull(){
translate ([-matrix_screw_x+matrix_offset,-hullheight]) cylinder(d=matrix_screw_radius+case_screw_spacing*2,h=layer_height, center=true);

translate ([-matrix_screw_x+matrix_offset,-matrix_screw_y]) cylinder(d=matrix_screw_radius+case_screw_spacing*2,h=layer_height, center=true);
}

}

module led_hole(){
difference(){
children();
translate ([led_matrix_offset+matrix_offset,0,0]) cube([led_matrix_width,led_matrix_height,layer_height], center=true);
}
}