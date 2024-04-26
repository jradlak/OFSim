import orbital_fs
import time

orbital_fs.orbital_thrust_change(0.24)

orbital_data = orbital_fs.get_orbital_data()

while float(orbital_data["altitude"]) < 10000 and not orbital_data["shouldStop"]:
    orbital_data = orbital_fs.get_orbital_data()
    print('rocket altitude: ', orbital_data["altitude"])
    time.sleep(0.5)