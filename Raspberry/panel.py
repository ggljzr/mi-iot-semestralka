from flask import Flask, url_for, redirect, render_template
import device as dev
from rpi_info import get_interface_address 

app = Flask(__name__)

led_board = dev.Led_board('A0A0A0A0C3','F0F0F0F0C3',2,'Ovládání LED')

@app.route("/")
def home_page():
    status = led_board.led_status();
    error = False;

    #ten vypis mozna presunout az do toho templatu
    #tady predavat jen error flag (mozna naky cislo misto jen true/false)
    error_message = 'Zkontrolujte připojení zařízení' 

    #tady by mozna bylo lepsi nevodchytavat ten Device_error
    #v board.led_status ale nechat ho dobublat az sem,
    #ale vono na tim asi zas tak nesejde
    #protoze tu chybovou hlasku tady stejne chcem trochu jinou
    if status == None:
        error = True
    
    ip_wlan = ['wlan', get_interface_address('wlan')]
    ip_eth0 = ['eth0', get_interface_address('eth0')]

    interfaces = [ip_eth0, ip_wlan]
    
    return render_template('home_page.html', 
            led_status=status, 
            led_name = led_board.name,
            error = error,
            error_message = error_message,
            interfaces = interfaces)

@app.route("/led/<led_num>/<value>")
def led_control(led_num, value):
    led_num = int(led_num)
    led_value = int(value)

    led_board.led_write(led_num, led_value)

    return redirect(url_for('home_page'))

if __name__ == "__main__":
    app.run(host='raspberrypi.local',debug=True)
