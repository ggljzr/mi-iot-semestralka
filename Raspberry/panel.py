from flask import Flask, url_for, redirect, render_template
import device as dev

app = Flask(__name__)


led_board = dev.Led_board('A0A0A0A0C3','F0F0F0F0C3',2,'led board')

@app.route("/")
def home_page():
    status = led_board.led_status();
    return render_template('home_page.html', led_status=status, led_name = led_board.name)

@app.route("/led/<led_num>/<state>")
def led_control(led_num, state):
    led_num = int(led_num)
    
    if state == 'on':
        led_board.led_on(led_num)
    elif state == 'off':
        led_board.led_off(led_num)

    return redirect(url_for('home_page'))

if __name__ == "__main__":
    app.run(host='raspberrypi.local',debug=True)
