%p1 = 17.4 % watts
p1 = 6 * 6.5; %watts
p2 = 8.4 % watts

p3 = p1+p2 % watts = joules/second

bat = 350 * 1e-3 % amp hours

coul_sec = bat * (60*60) % amp seconds, coulombs

joules = 6 * coul_sec % joules

time = joules/((p1+p2)) % time = joules / (joules/second) = seconds

need = 10/(time/60) * bat % Amps



