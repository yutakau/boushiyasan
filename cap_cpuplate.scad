/* cap_cpuplate */

// 2018/6/5  15:35   usui
t=1.2;
h=45;
w=50;

//base-plate
module baseplate(h,w,t) {
    difference() {
        cube([h,w,t]);
    
        // plate large-hole *2
        translate([8,12,0])      cube([8,26,20]);
        translate([h-10-6,12,0])  cube([8,26,20]);    
        
        // plate circle
        translate([5,5+t,0])   cylinder(20,2,center=true);
        translate([10,5+t,0])   cylinder(20,2,center=true);
        
        translate([h-5,5+t,0])   cylinder(20,2,center=true);
        translate([h-10,5+t,0])   cylinder(20,2,center=true);

        translate([5,w-7,0])   cylinder(20,2,center=true);
        translate([10,w-7,0])   cylinder(20,2,center=true);
        
        translate([h-5,w-7,0])   cylinder(20,2,center=true);
        translate([h-10,w-7,0])   cylinder(20,2,center=true);

    }
    

    //wall
    //wt=t+6;
    wt=t+7;
    translate([0,0,0])  cube([t,w,wt]);
    translate([h-t,0,0])
        difference() {
            cube([t,w,wt]);
            translate([0,25,5]) cube([10,12,10]); //USB
        }
    translate([0,w-t,0])  cube([h,t,wt]);
    translate([0,0,0])  cube([h,t,wt]);


    //table
    translate([h/2-4,w-t-5,0]) cube([10,5,3]);
};



//post
translate([0,0,8])  baseplate(h,w,t);

translate([0,0,0])  cube([6,4,8]);
translate([h-6,0,0])  cube([6,4,8]);

translate([0  ,w-4,8-4])  cube([6,4,4]);
translate([h-6,w-4,8-4])  cube([6,4,4]);
