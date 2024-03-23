import orbital_fs
import time

# wait for 1 second:
time.sleep(1)

orbital_fs.orbital_thrust_magnitude_change(0.24)

orbital_data = orbital_fs.get_orbital_data()

#waiting for the latitude to be less greater than 1500 m
print('!!!initial rocket altitude: ', orbital_data["altitude"]) 
while float(orbital_data["altitude"]) < 1.5 and not orbital_data["shouldStop"]:
    orbital_data = orbital_fs.get_orbital_data()
    print('rocket altitude: ', orbital_data["altitude"])    
    time.sleep(0.5)

#changing the rotation of the rocket:
print(' przed rotacja !!!!')
for i in range(0, 24):
    print(' wewnatrz petli !!!!')
    orbital_fs.orbital_rocket_rotation_change([1.27, -1.27, 0])
    print("rotating rocket")
    time.sleep(0.2)

while float(orbital_data["altitude"]) < 80 and not orbital_data["shouldStop"]:
    orbital_data = orbital_fs.get_orbital_data()
    time.sleep(0.2)

orbital_fs.orbital_thrust_magnitude_change(0.0001)