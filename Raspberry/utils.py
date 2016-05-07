import datetime
import matplotlib.pyplot as plt
from matplotlib import dates

#trida pro logovani dat do souboru
#po vytvoreni si v bufferu drzi max
#entries_count zaznamu a po pridani 
#novyho pripadne vymaze nejstarsi 
#a aktualizuje logfile
#data sou ukladany s timestampem

#jen bacha ze von na zacatku
#prepise vsechny data v tim souboru
#kdyz existuje, tj kdyz se to zapne
#tak se zacne logovat vod zacatku
class Logger(object):
    def __init__(self, logfile_path, entries_count):
        self.logfile_path = logfile_path
        self.entries_count = entries_count
        self.log_buffer = []

    def write_to_file(self):
        with open(self.logfile_path, 'w') as logfile:
            for entry in self.log_buffer:
                logfile.write('{} {}\n'.format(entry[0], entry[1]))
    
    def add_entry(self, value):
        
        now = datetime.datetime.now()
        new_entry = (now, value)
        
        self.log_buffer.append(new_entry)

        if len(self.log_buffer) > self.entries_count:
            self.log_buffer.pop(0)

        self.write_to_file()

    #zatim jednoduchy vykreslovani do grafu
    def plot_data(self, img_path, ylabel = 'Hodnota', time_format = None):
        x = [entry[0] for entry in self.log_buffer]
        y = [entry[1] for entry in self.log_buffer]

        plt.ylabel(ylabel)
        
        if time_format != None:
            plt.gca().xaxis.set_major_formatter(dates.DateFormatter(time_format))
            plt.gca().xaxis.set_major_locator(dates.DayLocator())
        
        plt.plot(x,y)
        plt.savefig(img_path)
        plt.clf()

