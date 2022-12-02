motors = (motor_1, motor_2)
angles = (-20, 45, 20, -45, 0)
solenoids = (solenoid_a, solenoid_b)
leds = (led_f, led_g, led_e)

def aim(degrees):
    motor_1.spin_to_position((degrees * 7), DEGREES, wait=False)
    motor_2.spin_to_position((degrees * 7), DEGREES, wait=True)

def write(text, fill_color=Color.BLACK, pen_color=Color.WHITE):
    brain.screen.set_cursor(1,1)
    brain.screen.set_fill_color(fill_color)
    brain.screen.set_pen_color(pen_color)
    brain.screen.draw_rectangle(0, 0, 1000, 1000)
    brain.screen.print(text)

def shoot(angle):
    write("Waiting...")
    while not limit_switch_h.pressing():
        wait(5, MSEC)

    for countdown in range(10, 0, -2):

        write("LAUNCHING IN " + str(countdown), Color.WHITE, Color.RED)
        (led.on() for led in leds)
        wait(0.1, SECONDS)

        write("LAUNCHING IN " + str(countdown-1), Color.RED, Color.WHITE)
        (led.off() for led in leds)
        wait(0.1, SECONDS)

    solenoid_a.set(True)
    write("Launched!")
    wait(1, SECONDS)
    solenoid_a.set(False)

    write("Aiming...")
    aim(angle + 10)
    aim(angle)

def main():
    brain.screen.set_font(FontType.MONO60)

    for motor in motors:
        motor.set_velocity(50, PERCENT)
        motor.set_stopping(BRAKE)
        motor.set_max_torque(100, PERCENT)

    for angle in angles:
        shoot(angle)

main()
