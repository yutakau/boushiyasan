/* cap_ledplate */

// 2018/6/5  16:09   usui
t=1;
h=16;
w=12;


difference() {
    cube([h,w,t]);
    
    // plate circle
    translate([3+t  ,3+t  ,0])   cylinder(20,2,center=true);
    translate([h-3-t,3+t  ,0])   cylinder(20,2,center=true);
    translate([3+t  ,w-3-t,0])   cylinder(20,2,center=true);
    translate([h-3-t,w-3-t,0])   cylinder(20,2,center=true);    
};

//wall
wt=t+2.5;

module wall(h,w,t){
    translate([0,0,0])  cube([t,w,wt]);
    translate([h-t,0,0])  cube([t,w,wt]);
    translate([0,w-t,0])  cube([h,t,wt]);
    translate([0,0,0])  cube([h,t,wt]);
};

difference() {
    wall(h,w,t);
    rotate([0,90,0])
    translate([-3.5,w/2,-10])
    cylinder(50,1.3,1.3);
}




