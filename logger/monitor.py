# Writes measurement and status reports of the Arduino to an SQLite DB
# This does the trick for now, but don't use it to learn Python

import serial, time, datetime, sqlite3

database = sqlite3.connect('Aquarium.db')
cursor = database.cursor()
cursor.execute('''CREATE TABLE IF NOT EXISTS log
             (date text, time text, epoch real, temp real, pH real, o2 real)''')
cursor.execute('''CREATE TABLE IF NOT EXISTS config
             (key text, value text, epoch real)''')
database.commit()
database.close()

serial = serial.Serial('/dev/ttyACM0', 38400)
serial.flushInput()

temp = '0.0'
pH = '0.0'
o2 = '0.0'
while 1:
    try:
        line = serial.readline()
    except Exception as inst:
        print inst
    print(line),
    if line[:2] == 'TE':
        temp = line[3:]
    if line[:2] == 'PH':
        pH = line[3:]
    if line[:2] == 'O2':
        o2 = line[3:]
        try:
            database = sqlite3.connect('Aquarium.db')
            cursor = database.cursor()
            cursor.execute("INSERT INTO log VALUES (?,?,?,?,?,?)", (datetime.datetime.now().strftime("%Y-%m-%d"), datetime.datetime.now().strftime("%H:%M:%S"), time.mktime(time.localtime()), temp, pH, o2))
            database.commit()
            database.close()
        except Exception as inst:
            print inst
    if line[:2] == 'TI':
        try:
            database = sqlite3.connect('Aquarium.db')
            cursor = database.cursor()
            cursor.execute("DELETE FROM config WHERE key=?", ("time", ))
            cursor.execute("INSERT INTO config VALUES (?,?,?)", ("time", line[3:], time.mktime(time.localtime())))
            database.commit()
            database.close()
        except Exception as inst:
            print inst
    if line[:2] == 'ST':
        try:
            database = sqlite3.connect('Aquarium.db')
            cursor = database.cursor()
            cursor.execute("DELETE FROM config WHERE key=?", ("status", ))
            cursor.execute("INSERT INTO config VALUES (?,?,?)", ("status", line[3:], time.mktime(time.localtime())))
            database.commit()
            database.close()
        except Exception as inst:
            print inst
    time.sleep(1)

