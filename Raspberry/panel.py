from flask import Flask, url_for, redirect, render_template
import device as dev

app = Flask(__name__)


board = dev.Led_board('aa','bb',0,'led board')

@app.route("/")
def home_page():
    status = board.led_status();
    return render_template('home_page.html', led_status=status)

@app.route("/led/<led_num>/<state>")
def board_led_control(led_num, state):
    led_num = int(led_num)
    
    if state == 'on':
        board.led_on(led_num)
    elif state == 'off':
        board.led_off(led_num)

    return redirect(url_for('home_page'))

if __name__ == "__main__":
    app.run(debug=True)
