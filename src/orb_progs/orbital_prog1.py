import orbital_fs

print('in python: ')

val = orbital_fs.orbital_thrust_get()
print('in python:orbital_fs.orbital_thrust_get() returned ', val)

orbital_fs.orbital_thrust_change(val["angle"]*20+80)