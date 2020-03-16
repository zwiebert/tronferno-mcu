import sys, serial
import serial.tools.list_ports as list_ports

nmb_args = len(sys.argv) - 1;
ser = 0
ser_port = "/dev/ttyUSB0" if (nmb_args < 1) else sys.argv[1]  
ser_baud = 115200

NONE = 0
OK = 1
FAIL = 2

def parse_test_output():
    lines = ser.readlines()
    for line in lines:
        line = line.decode('utf-8')
        #print(line)
        if (line == "OK\r\n"):
            return OK
        if (line == "FAIL\r\n"):
            return FAIL
        if (":FAIL:" in line):
            print(line);
        if ("assert failed!" in line):
            print(line);
            return FAIL
        if ("Guru Meditation Error:" in line):
            print(line);
            return FAIL
    return NONE



def run_test(name):
    """
    NAME - name of test or asterisk to run all tests
    """
    global ser
    res = NONE
    ser = serial.Serial(ser_port, ser_baud, timeout=1)
    
    lines = ser.readlines() # mcu init lines + PRESS ENTER ... propmpt
        
    cmd = "*\r\n"
    ser.write(cmd.encode('utf-8'))
    
    while (True):
        res = parse_test_output()
        if (res != NONE):
            break    
    ser.close()
    return (res == OK)
    
    
def try_run_test(name):
    try:
        
       return run_test(name)
                 
    except serial.serialutil.SerialException as e:
        print("cannot open serial port " + ser_port +"\nPorts available are:")
        ser_list = sorted(ports.device for ports in list_ports.comports())
        print(ser_list)
        return False
    except Exception as e:
        print("unsuccessful: %s" % e)
        return False
    
    
tests_ok = try_run_test("*")

sys.exit(0 if (tests_ok) else 1)

