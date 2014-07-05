# Consolidats measurement data and deletes old data logs
# Should be run by crontab once a day, e.g. after midnight
# 0 5 * * * sudo python /home/pi/consolidate.py
# This does the trick for now, but don't use it to learn Python

import serial, time, sqlite3, datetime

database = sqlite3.connect('/home/pi/Aquarium.db')
cursor = database.cursor()
cursor.execute('''CREATE TABLE IF NOT EXISTS daily
             (date text, temp real, pH real, o2 real)''')

yesterday = datetime.date.today() - datetime.timedelta(1)

result = cursor.execute("SELECT avg(temp) FROM log WHERE date=?", (yesterday.strftime("%Y-%m-%d"),)).fetchone()
if result is None or result[0] is None:
    avg_temp = 0.0
else:
    avg_temp = float(result[0])
    
result = cursor.execute("SELECT avg(pH) FROM log WHERE date=?", (yesterday.strftime("%Y-%m-%d"),)).fetchone()
if result is None or result[0] is None:
    avg_pH = 0.0
else:
    avg_pH = float(result[0])
    
result = cursor.execute("SELECT avg(o2) FROM log WHERE date=?", (yesterday.strftime("%Y-%m-%d"),)).fetchone()
if result is None or result[0] is None:
    avg_o2 = 0.0
else:
    avg_o2 = float(result[0])
    
if avg_temp > 0:
    cursor.execute("DELETE FROM daily WHERE date=?", (yesterday.strftime("%Y-%m-%d"),))
    cursor.execute("INSERT INTO daily VALUES (?,?,?,?)", (yesterday.strftime("%Y-%m-%d"), avg_temp, avg_pH, avg_o2))
    for i in range(2, 30):
        day = datetime.date.today() - datetime.timedelta(i)
        cursor.execute("DELETE FROM log WHERE date=?", (day.strftime("%Y-%m-%d"),))

cursor.execute("VACUUM")

database.commit()
database.close()