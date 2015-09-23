$fn=100;
cube([54,60,5],center=true);

difference(){
translate([0,0,42.5]) cube([54,5,85],center=true);

translate([0,0,80]) rotate([90,0,0]) translate([13.25,0,0]) cylinder(d=4, h=20,center=true);
translate([0,0,80]) rotate([90,0,0]) translate([-13.25,0,0]) cylinder(d=4, h=20,center=true);  
    
translate([0,0,80-44]) rotate([90,0,0]) translate([13.25,0,0]) cylinder(d=4, h=20,center=true);
translate([0,0,80-44]) rotate([90,0,0]) translate([-13.25,0,0]) cylinder(d=4, h=20,center=true);  
    
    
    
    
    translate([0,0,70]) rotate([90,0,0]) translate([21.75,0,0]) cylinder(d=4, h=20,center=true);
translate([0,0,70]) rotate([90,0,0]) translate([-21.75,0,0]) cylinder(d=4, h=20,center=true);  
    
translate([0,0,70-20.2]) rotate([90,0,0]) translate([21.75,0,0]) cylinder(d=4, h=20,center=true);
translate([0,0,70-20.5]) rotate([90,0,0]) translate([-21.75,0,0]) cylinder(d=4, h=20,center=true); 
    
    
    translate([0,0,10]) cube([15,30,4.5], center=true);
}