
import subprocess as sb

#taky funguje jako univerzalni zarizeni, podporuje
#vsechny prikazy, kdytak to bude akorat kricet ze 
#funkce neni implementovana
class Device(object):
    def __init__(reading_pipe, writing_pipe, dev_num, name):
        self.reading_pipe = reading_pipe #reading pipe to rpi
        self.writing_pipe = writing_pipe #writing pipe to rpi
        self.dev_num = dev_num           #device number
        self.name = name
        self.commands = [("LED toggle", 0x01), ("LED on", 0x02), ("LED off", 0x03), ("LED status", 0x04)]
    
    def send_cmd(self, cmd_num, cmd_data):
        output = sb.check_output(["./rpi_rf24", self.reading_pipe, self.writing_pipe, cmd_num, cmd_data])
        return self.parse_output(output)

    def parse_output(out_string):
        out_string = out_string.split(" ")
        out_bytes = []
        for byte in out_string:
            out_bytes.append(int(byte, 16))

        return out_bytes


class Led_board(Device):
    def __init__(reading_pipe, writing_pipe, dev_num, name):
        Device.__init__(reading_pipe, writing_pipe, dev_num, name)
        self.commands = [("LED on", 0x02), ("LED off", 0x03), ("LED status", 0x04)]

    def led_on(self, led_num):
        out = self.send_cmd(0x02, led_num)
        return out

    def led_off(self, led_num):
        out = self.send_cmd(0x03, led_num)
        return out

    def led_status(self)
        out = self.send_cmd(0x04, 0)
        return out


