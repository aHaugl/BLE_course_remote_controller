### Method 1 - Optional steps: Modify the pwm_led0 instance to drive the servo motor
This part is to showcase how you can reconfigure existing peripheral devices such as the predefined pwm_led0 instance to something else. It is a section that is defined to break apart some of the abstraction layers that comes with overlayfiles and devicetree.

Now to the steps for how to do this: If you open your nrf52840dk_nrf52840.dts, which is our standard board file, we can see what pwm_led0 looks like by default:

[pwm_led device](https://github.com/nrfconnect/sdk-zephyr/blob/93ad09a7305328387936b68059b63f64efd44f60/boards/arm/nrf52840dk_nrf52840/nrf52840dk_nrf52840.dts#L48-L53)  default configuration | 
------------ |
<img src="https://github.com/aHaugl/OV_Orbit_BLE_Course/blob/main/images/Step3.3.png" width="1000"> |

```
	pwmleds {
		compatible = "pwm-leds";
		pwm_led0: pwm_led_0 {
			pwms = <&pwm0 0 PWM_MSEC(20) PWM_POLARITY_INVERTED>;
		};
	};
```

As mentioned in the `overlay files` section , we don't want to change anything inside this file, because those changes will stick to all other projects that are using the same board file. This is why we want to do the changes in our overlay file. Unfortunately, the pin number is not set here directly. It is set in &pwm0 inside pwm_led0. But since the default configuration for pwm_led0 is PWM_POLARITY_INVERTED, and we want to change that as well, we need to add the pwmleds snippet to our overlay file as well. 
Let us start by adding a pwmleds snippet to our `nrf52840dk_nrf52840.overlay` file. This will overwrite the default settings from the .dts file.

```C
/{
    pwmleds {
        compatible = "pwm-leds";
        pwm_led0: pwm_led_0 {
            pwms = <&pwm0 0 PWM_MSEC(20) PWM_POLARITY_NORMAL>;
        };
    };
};
```

You can see here that the only change we did was to change the polarity from inverted to normal. This means that the PWM signal will have a high output for the duty cycle, instead of low. If you right click and select "go to definition" on the `&pwm0` in your overlay file, you will see something like this:

```C
&pwm0 {
	status = "okay";
	pinctrl-0 = <&pwm0_default>;
	pinctrl-1 = <&pwm0_sleep>;
	pinctrl-names = "default", "sleep";
};
```

So our pin numbers are set in pwm0_default and pwm0_sleep. Let us start by changing the names of these as we add this to our `overlay` file:

```C
&pwm0 {
    status = "okay";
    pinctrl-0 = <&pwm0_custom>;
    pinctrl-1 = <&pwm0_csleep>;
    pinctrl-names = "default", "sleep";
};
```

You can call them whatever you like. I used pwm0_custom and pwm0_csleep. The last part we need to do is that we need to define pwm0_custom and pwm0_csleep. Try right clicking the pwm0_default and pwm0_sleep in the .dts file to see what they look like by default:

```C
&pinctrl {
	pwm0_default: pwm0_default {
		group1 {
			psels = <NRF_PSEL(PWM_OUT0, 0, 3)>;
			nordic,invert;
		};
	};
    
	pwm0_sleep: pwm0_sleep {
		group1 {
			psels = <NRF_PSEL(PWM_OUT0, 0, 3)>;
			low-power-enable;
		};
	};
};
```

Note that I added the `&pinctrl {` and `};` at the top and bottom, since we need this when we copy it into our `.overlay` file. 

Add this to your `.overlay` file (with the names that you used in `&pwm0`):

```C
&pinctrl {
    pwm0_custom: pwm0_custom {
        group1 {
            psels = <NRF_PSEL(PWM_OUT0, 0, 3)>;
            nordic,invert;
        };
    };

    pwm0_csleep: pwm0_csleep {
        group1 {
            psels = <NRF_PSEL(PWM_OUT0, 0, 3)>;
            low-power-enable;
        };
    };
};
```

FYI: the `0, 3` is port 0, pin 3. If you wanted to use e.g. pin P1.15, you would set `psels = <NRF_PSEL(PWM_OUT0, 1, 15). 

In the end, your `nrf52840dk_nrf52840.overlay` file should look something like this:

```C
&pinctrl {
    pwm0_custom: pwm0_custom {
        group1 {
            psels = <NRF_PSEL(PWM_OUT0, 0, 3)>;
            nordic,invert;
        };
    };

    pwm0_csleep: pwm0_csleep {
        group1 {
            psels = <NRF_PSEL(PWM_OUT0, 0, 3)>;
            low-power-enable;
        };
    };
};

&pwm0 {
    status = "okay";
    pinctrl-0 = <&pwm0_custom>;
    pinctrl-1 = <&pwm0_csleep>;
    pinctrl-names = "default", "sleep";
};

/{
    pwmleds {
        compatible = "pwm-leds";
        pwm_led0: pwm_led_0 {
            pwms = <&pwm0 0 PWM_MSEC(20) PWM_POLARITY_NORMAL>;
        };
    };
};
```

Try to connect the servo motor. It has three wires. One brown, which you can connect to GND. Then you have one Red, which you can connect to VDD (not the one marked 5V), and then connect the yellow/orange wire to whatever pin you chose for your PWM pin (probably P0.03). 
Does the motor move? **Warning: Do not attempt to move the rotor by force while the motor is connected to power. The motors are fragile...**

If it does, you can try to create a function inside motor_control.c that you can call from e.g. the button handler to set the pwm signal to different values between 1ms and 2ms. These motors are cheap, so some motors goes 180 degrees between 1ms and 2ms, but yout milage may vary. Try out different values to see what the limits are for your motor. When I tested one of the motors, it turned out that the limits were 0.4ms and 2.4ms. 
Call the function `set_motor_angle()` and make it return an int (0 on success, negative value on error). Declare it in motor_control.h, and implement it in motor_control.c. make it have an input parameter either as a PWM duty cycle, or an input angle (degrees between 0 and 180).

Use this to set different angles, depending on what button you pressed. 

If you are having problems with controlling the motors, you can have a look at what my motor_control.h and motor_control.c looks like at this point in time in [the solution for step 3, method 1](https://github.com/aHaugl/OV_Orbit_BLE_Course/tree/main/temp_files/Step_3.1_sol).
