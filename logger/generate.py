# Generates a data and status report to be delivered by a web server
# This does the trick for now, but don't use it to learn Python

import serial, time, sqlite3, datetime, pygal, os
from tempfile import NamedTemporaryFile

database = sqlite3.connect('Aquarium.db')
cursor = database.cursor()

file = open('/var/www/index-new.html', 'w')
file.write('<html><body bgcolor="#000000">')
file.write('<h1 style="font-family:Courier;color:#FFFFFF;">Aquarium Monitoring</h1>')
file.write('<h2 style="font-family:Courier;color:#FFFFFF;">Last Measurement</h2>')
file.write('<ul style="font-family:Courier;color:#FFFFFF;list-style-type: none;">')

result = cursor.execute("SELECT * FROM log ORDER BY epoch DESC LIMIT 1").fetchone()
if result is None:
    file.write('<li>No measurement.</li>');   
else:
    file.write('<li>' + 'Recorded on: ' + result[0] + ', ' + result[1] + '</li>');
    file.write('<li>' + 'Temperature: ' + str(result[3]) + 'C</li>');
    file.write('<li>' + 'pH: ' + str(result[4]) + '</li>');
    file.write('<li>' + 'Oxygen: ' + str(result[5]) + '</li>');
    result = cursor.execute("SELECT * FROM config WHERE key=? LIMIT 1", ("time", )).fetchone()
    if not result is None:
        file.write('<li>' + 'Last time report by controller: ' + str(result[1]) + '</li>');   
    result = cursor.execute("SELECT * FROM config WHERE key=? LIMIT 1", ("status", )).fetchone()
    if not result is None:
        file.write('<li>' + 'Last status report by controller: ' + str(result[1]) + ' at ' + time.strftime('%H:%M:%S', result[2]) + '</li>');

file.write('</ul>')
file.write('<h2 style="font-family:Courier;color:#FFFFFF;">Last 24 Hours</h2>')

avg_temp = []
avg_pH = []
avg_o2 = []

for h in range(24, 0, -1):
    start = time.mktime((datetime.datetime.today() - datetime.timedelta(hours = h)).timetuple())
    end = time.mktime((datetime.datetime.today() - datetime.timedelta(hours = h-1)).timetuple())
    
    result = cursor.execute("SELECT avg(temp) FROM log WHERE epoch>=? AND epoch<=?", (start, end)).fetchone()
    if result is None or result[0] is None:
        avg_temp.append(0.0)
    else:
        avg_temp.append(float(result[0]))
        
    result = cursor.execute("SELECT avg(pH) FROM log WHERE epoch>=? AND epoch<=?", (start, end)).fetchone()
    if result is None or result[0] is None:
        avg_pH.append(0.0)
    else:
        avg_pH.append(float(result[0]))
        
    result = cursor.execute("SELECT avg(o2) FROM log WHERE epoch>=? AND epoch<=?", (start, end)).fetchone()
    if result is None or result[0] is None:
        avg_o2.append(0.0)
    else:
        avg_o2.append(float(result[0]))
    
config = pygal.Config()
config.show_legend = False

custom_css = '''
  svg {
      max-height: 100%;
  }
  {{ id }}.guides text {
      font-size: 16px;
  }
  {{ id }}.guides text.major {
      font-size: 16px;
  }
  {{ id }}.title {
      font-size: 32px;
  }
  {{ id }}.dot {
    fill: #FF5995;
    stroke-width: 10px;
  }
  {{ id }}.color-0 {
    stroke: #FF5995;
    fill: #FF5995;
    stroke-width: 6px;
  }
'''
custom_css_file = '/var/www/temp.css'
with open(custom_css_file, 'w') as f:
  f.write(custom_css)
config.css.append(custom_css_file)
  
file.write('<figure class="temp" style="width:30%;display: inline-block;margin: 8px 10px 8px 10px;">')
line_chart = pygal.Line(config)
line_chart.title = 'Temperature'
line_chart.x_labels = map(str, range(1, 24))
line_chart.add('Temperature', avg_temp)
file.write(line_chart.render())    
file.write('</figure>')

custom_css = '''
  svg {
      max-height: 100%;
  }
  {{ id }}.guides text {
      font-size: 16px;
  }
  {{ id }}.guides text.major {
      font-size: 16px;
  }
  {{ id }}.title {
      font-size: 32px;
  }
  {{ id }}.dot {
    fill: #B6E354;
    stroke-width: 10px;
  }
  {{ id }}.color-0 {
    stroke: #B6E354;
    fill: #B6E354;
    stroke-width: 6px;
  }
'''
custom_css_file = '/var/www/pH.css'
with open(custom_css_file, 'w') as f:
  f.write(custom_css)
config.css.append(custom_css_file)

file.write('<figure class="pH" style="width:30%;display: inline-block;;margin: 8px 10px 8px 10px;">')
line_chart = pygal.Line(config)
line_chart.title = 'pH'
line_chart.x_labels = map(str, range(1, 24))
line_chart.add('pH',  avg_pH)
file.write(line_chart.render())    
file.write('</figure>')

custom_css = '''
  svg {
      max-height: 100%;
  }
  {{ id }}.guides text {
      font-size: 16px;
  }
  {{ id }}.guides text.major {
      font-size: 16px;
  }
  {{ id }}.title {
      font-size: 32px;
  }
  {{ id }}.dot {
    fill: #8CEDFF;
    stroke-width: 10px;
  }
  {{ id }}.color-0 {
    stroke: #8CEDFF;
    fill: #8CEDFF;
    stroke-width: 6px;
  }
'''
custom_css_file = '/var/www/o2.css'
with open(custom_css_file, 'w') as f:
  f.write(custom_css)
config.css.append(custom_css_file)

file.write('<figure class="o2" style="width:30%;display: inline-block;;margin: 8px 10px 8px 10px;">')
line_chart = pygal.Line(config)
line_chart.title = 'Oxygen'
line_chart.x_labels = map(str, range(1, 24))
line_chart.add('Oxygen', avg_o2)
file.write(line_chart.render())    
file.write('</figure>')

avg_temp = []
avg_pH = []
avg_o2 = []

query = cursor.execute("SELECT * FROM daily ORDER BY date DESC LIMIT 30")
result = query.fetchone()
while not (result is None or result[0] is None):
    avg_temp.insert(0, float(result[1]))
    avg_pH.insert(0, float(result[2]))
    avg_o2.insert(0, float(result[3]))
    result = query.fetchone()
    
file.write('<h2 style="font-family:Courier;color:#FFFFFF;">Last 30 Days</h2>')    
    
custom_css = '''
  svg {
      max-height: 100%;
  }
  {{ id }}.guides text {
      font-size: 16px;
  }
  {{ id }}.guides text.major {
      font-size: 16px;
  }
  {{ id }}.title {
      font-size: 32px;
  }
  {{ id }}.dot {
    fill: #FF5995;
    stroke-width: 10px;
  }
  {{ id }}.color-0 {
    stroke: #FF5995;
    fill: #FF5995;
    stroke-width: 6px;
  }
'''
custom_css_file = '/var/www/temp.css'
with open(custom_css_file, 'w') as f:
  f.write(custom_css)
config.css.append(custom_css_file)
  
file.write('<figure class="temp" style="width:30%;display: inline-block;;margin: 8px 10px 8px 10px;">')
line_chart = pygal.Line(config)
line_chart.title = 'Temperature'
line_chart.x_labels = map(str, range(1, 24))
line_chart.add('Temperature', avg_temp)
file.write(line_chart.render())    
file.write('</figure>')

custom_css = '''
  svg {
      max-height: 100%;
  }
  {{ id }}.guides text {
      font-size: 16px;
  }
  {{ id }}.guides text.major {
      font-size: 16px;
  }
  {{ id }}.title {
      font-size: 32px;
  }
  {{ id }}.dot {
    fill: #B6E354;
    stroke-width: 10px;
  }
  {{ id }}.color-0 {
    stroke: #B6E354;
    fill: #B6E354;
    stroke-width: 6px;
  }
'''
custom_css_file = '/var/www/pH.css'
with open(custom_css_file, 'w') as f:
  f.write(custom_css)
config.css.append(custom_css_file)

file.write('<figure class="pH" style="width:30%;display: inline-block;;margin: 8px 10px 8px 10px;">')
line_chart = pygal.Line(config)
line_chart.title = 'pH'
line_chart.x_labels = map(str, range(1, 24))
line_chart.add('pH',  avg_pH)
file.write(line_chart.render())    
file.write('</figure>')

custom_css = '''
  svg {
      max-height: 100%;
  }
  {{ id }}.guides text {
      font-size: 16px;
  }
  {{ id }}.guides text.major {
      font-size: 16px;
  }
  {{ id }}.title {
      font-size: 32px;
  }
  {{ id }}.dot {
    fill: #8CEDFF;
    stroke-width: 10px;
  }
  {{ id }}.color-0 {
    stroke: #8CEDFF;
    fill: #8CEDFF;
    stroke-width: 6px;
  }
'''
custom_css_file = '/var/www/o2.css'
with open(custom_css_file, 'w') as f:
  f.write(custom_css)
config.css.append(custom_css_file)

file.write('<figure class="o2" style="width:30%;display: inline-block;;margin: 8px 10px 8px 10px;">')
line_chart = pygal.Line(config)
line_chart.title = 'Oxygen'
line_chart.x_labels = map(str, range(1, 24))
line_chart.add('Oxygen', avg_o2)
file.write(line_chart.render())    
file.write('</figure>')    

file.write('</body></html>')
file.close()

try:
    os.remove('/var/www/index.html')
except:
    print 'index.html not found, maybe next time'
os.rename('/var/www/index-new.html', '/var/www/index.html')

database.close()