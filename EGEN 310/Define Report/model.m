% Anthony Louis Rosenblum
% ECE Mathematical Model
% for EGEN-310R
% Team E7
% Updated 2/6/2019

%% Section 1: Calculations for Discovery Report
% Last updated 1/29/2019

%p1 = 17.4 % watts
p1 = 6 * 6.5; %watts
p2 = 8.4 % watts

p3 = p1+p2 % watts = joules/second

bat = 350 * 1e-3 % amp hours

coul_sec = bat * (60*60) % amp seconds, coulombs

joules = 6 * coul_sec % joules

time = joules/((p1+p2)) % time = joules / (joules/second) = seconds

need = 10/(time/60) * bat % Amps

%% Section 2: Calculations for Definition Report
% Last updated 2/6/2019

% Using design requests from ME/METs

% Assume 4 motors, each controlling one wheel, no servos in system

% Determine necessary functions and how they could be nested

% System Design 1

wheels = [[L1,R1],[L2,R2]];

% Forward 

L1.fwd
R1.fwd
L2.fwd
R2.fwd

% Reverse

L1.rvr
R1.rvr
L2.rvr
R2.rvr

% Turn left in place

L1.rvr
R1.fwd
L2.rvr
R2.fwd

% Turn right in place

L1.fwd
R1.rvr
L2.fwd
R2.rvr

% System 1 requires 4 functions, all at top level

% System 2

left_wheels = [L1,L2]
right_wheels = [R1,R2]

left_wheels.fwd = [x.fwd,y.fwd]
left_wheels.rvr = [x.rvr,y.rvr]

right_wheels.fwd = [x.fwd,y.fwd]
right_wheels.rvr = [x.rvr,y.rvr]

% Forward
left_wheels.fwd
right_wheels.fwd

% Reverse
left_wheels.rvr
right_wheels.rvr

% Turn left in place
left_wheels.rvr
right_wheels.fwd

% Turn right in place
left_wheels.fwd
right_wheels.rvr

% System 2 requires 8 functions, 4 at top level, 4 nested underneath

% System 3

while (go = 1):
    left_wheels.fwd
    right_wheels.fwd
    BRA down
end

while (turn = 1):
    left_wheels.rvr
    right_wheels.fwd
    BRA down
end

while (go = -1):
    left_wheels.rvr
    right_wheels.rvr
    BRA down
end    

while (turn = -1):
    left_wheels.fwd
    right_wheels.rvr
    BRA top
end

% System 3 requires 4 while loops, and 4 functions nested underneath
