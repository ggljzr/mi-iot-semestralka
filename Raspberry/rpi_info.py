import subprocess as sb

#vraci string s ip adresou zvolenyho interface
#nebo None kdyz interface zadnou nema
def get_interface_address(interface):
    output = None
    
    try:
        output = sb.check_output(['ifconfig', str(interface)])
    except sb.CalledProcessError as e:
        print(str(e))
        return None

    ip_address = None
   
    output = output.decode('utf-8')
    output = output.split('\n')

    for line in output:
        if 'inet addr' in line:
            ip_address = line
   
    if ip_address == None:
        return None

    ip_address = ip_address.strip()
    ip_address = ip_address.split(" ")
    ip_address = ip_address[1].split(":")[1]

    return ip_address

