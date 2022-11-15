import RPi.GPIO as GPIO

# Setting up buzzer for indication
BUZZPIN = 5
GPIO.setmode(GPIO.BCM)
GPIO.setwarnings(False)
GPIO.setup(BUZZPIN, GPIO.OUT)


# Signaling start
GPIO.output(BUZZPIN,GPIO.HIGH)
sleep(0.1)
GPIO.output(BUZZPIN,GPIO.LOW)
