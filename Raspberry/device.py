
import subprocess as sb

LED_COUNT = 4

LED_ON = 0x02
LED_OFF = 0x03
LED_STATUS = 0x04

RESPONSE_OK = 0x00
RESPONSE_NOT_SUPPORTED = 0xFF

ERROR_FAILED_TO_RESPOND = "Device failed to respond"
ERROR_CMD_NOT_SUPPORTED = "Command is not supported on targeted device"

def parse_output(out_bstring):
        out_string = out_bstring.decode('utf-8')
        out_string = out_string.split(' ')
        out_bytes = []
        for byte in out_string:
            out_bytes.append(int(byte, 16))

        return out_bytes

class Device_error(Exception):
    def __init__(self, reading_pipe, writing_pipe, dev_num, cmd_num, msg):
        self.reading_pipe = reading_pipe
        self.writing_pipe = writing_pipe
        self.dev_num = dev_num;
        self.cmd_num = cmd_num
        self.msg = msg

    def __str__(self):
        error_str = 'Error raised when sending cmd={:x} on device={:x} using reading pipe={} and writing pipe={}, message: {} '.format(self.cmd_num, self.dev_num, self.reading_pipe, self.writing_pipe, self.msg)
        return error_str

#taky funguje jako univerzalni zarizeni, podporuje
#vsechny prikazy, kdytak to bude akorat kricet ze 
#funkce neni implementovana
class Device(object):
    def __init__(self, reading_pipe, writing_pipe, dev_num, name):
        self.reading_pipe = reading_pipe #reading pipe to rpi
        self.writing_pipe = writing_pipe #writing pipe to rpi
        self.dev_num = dev_num           #device number of targeted device (not rpi)
        self.name = name
    
    def send_cmd(self, cmd_num, cmd_data):
        cmd = ['sudo','./rpi_rf24', self.reading_pipe, self.writing_pipe, str(cmd_num), str(cmd_data)]

        output = sb.check_output(cmd)

        if len(output) == 0:
            raise Device_error(self.reading_pipe,self.writing_pipe, self.dev_num, cmd_num, ERROR_FAILED_TO_RESPOND)
            return None

        parsed_output = parse_output(output)

        if parsed_output[0] == RESPONSE_NOT_SUPPORTED:
            raise Device_error(self.reading_pip, self.writing_pipe, self.dev_num, cmd_num, ERROR_CMD_NOT_SUPPORTED)
            return None

        return parsed_output
    

class Led_board(Device):
    def __init__(self, reading_pipe, writing_pipe, dev_num, name):
        Device.__init__(self,reading_pipe, writing_pipe, dev_num, name)
        self.led_count = LED_COUNT

    #navratovy hodnoty sou pole bytu vod programu rpi_rf24
    #kdyz to vrati None, neco se nepovedlo
    #(bude raisnutej Device_error s infem)
    #jinak tady se ty errory proste tisknou
    #na stdout a jede se dal, taky by se mohly
    #nechat bublat az nahoru
    def led_on(self, led_num):
        try:
            out = self.send_cmd(0x02, led_num)
        except Device_error as e:
            print(str(e))

        print("on: {}".format(led_num))
        return out

    def led_off(self, led_num):
        try:
            out = self.send_cmd(0x03, led_num)
        except Device_error as e:
            print(str(e))

        print("off: {}".format(led_num))
        return out

    #vraci dvojici (cislo_led, sviti/nesviti)
    #kdyz je Device_error tak ho to vypise na
    #stdout a ve webovym rozhrani to hlasi chyba
    def led_status(self):
        try:
            out = self.send_cmd(0x04, 0)
        except Device_error as e:
            print(str(e))
            status_verb = ['chyba'] * self.led_count
        else:
            status = out[6:10]
            status_verb = ["svítí" if led == 0x01  else "nesvítí" for led in status]
        
        status_verb = zip(range(0,self.led_count), status_verb)

        return status_verb


