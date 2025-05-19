import socket
import time
from nicegui import events,ui
import numpy as np
import math as m


def Rx(theta):
    return np.matrix([[ 1, 0           , 0           ], [ 0, m.cos(theta),-m.sin(theta)], [ 0, m.sin(theta), m.cos(theta)]])

def Ry(theta):
    return np.matrix([[ m.cos(theta), 0, m.sin(theta)], [ 0           , 1, 0           ], [-m.sin(theta), 0, m.cos(theta)]])

def Rz(theta):
    return np.matrix([[ m.cos(theta), -m.sin(theta), 0 ], [ m.sin(theta), m.cos(theta) , 0 ], [ 0           , 0            , 1 ]])

UDP_IP = "127.0.0.1"
UDP_PORT = 5000

frame = 0
timestamp = 0
x = 0
y = 0
time_interval = 0.1
b = 0.000
button_value = 0

sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

def update_values(e):
    global x,y
    x = e.x
    y = e.y

def end_values(e):
    global x,y
    x = 0
    y = 0
    
def buttonActive(b):
    global button_value
    #if button_value == 0.0 :
    #    button_value = b
    #else:
    #   button_value = 0.0
    button_value = button_value ^ b
    
   
  


def send_values():
    global sock, frame, timestamp,x,y

    x_a = (knob_yaw.value / 360.0) * 6.28
    y_a = (knob_pitch.value / 360.0) * 6.28
    z_a = (knob_roll.value / 360.0) * 6.28

    matrix = Rx(x_a) * Ry(y_a) * Rz(z_a)
    

    #print(str(matrix) + "\n")

    MESSAGE = "fr " + str(frame) + "\n"
    MESSAGE += "ts " + str(timestamp) + "\n"
    MESSAGE += "6dcal 2\n"
    MESSAGE += "6df2 1 1 [0 1.000 9 3][1.000 1.000 1.000][" + str(matrix.item(0)) + " " + str(matrix.item(1)) + " " + str(matrix.item(2)) + " " + str(matrix.item(3)) + " " + str(matrix.item(4)) + " " + str(matrix.item(5)) + " " + str(matrix.item(6)) + " " + str(matrix.item(7)) + " " + str(matrix.item(8)) + "]["+ str(button_value) +" " + str(x) + " " + str(y) + " " + "0" + "]\n"
    MESSAGE += "6d 1 [0 1.000][" + str(slider_x.value * 10.0) + " " + str(slider_y.value * 10.0) + " " + str(slider_z.value * 10.0) + " 0.000 0.000 0.000][1.000 0.000 0.000 0.000 1.000 0.000 0.000 0.000 1.000]\n"
    MESSAGE += "3d 0\n"



    #print(MESSAGE)

    sock.sendto(bytes(MESSAGE, "ascii"), (UDP_IP, UDP_PORT))

    frame += 1
    timestamp += time_interval

def drag(e: events.SceneDragEventArguments):
    print("x: " + str(e.x) + "   y: " + str(e.y) + "   z: " + str(e.z))

sphere = None
know_yaw = None
know_pitch = None
knob_roll = None

def knob_change():
    global arrow,know_yaw, know_pitch, knob_roll
    arrow.rotate((knob_yaw.value / 360.0) * 6.28, (knob_pitch.value / 360.0) * 6.28, (knob_roll.value / 360.0) * 6.28)




ui.timer(time_interval, lambda: send_values())

def slider_change():
    global sphere
    sphere.move(slider_x.value * 0.01, slider_y.value * 0.01, slider_z.value * 0.01)
    scene.update()

with ui.scene(width=800, height=300) as scene:
    scene.box(wireframe=True).material('#888888').scale(3,3,3).move(x=0,y=0,z=1.5)
    sphere = scene.sphere().scale(0.3)
    arrow = scene.extrusion([[-0.2, 0], [0.0, 1], [0.2, 0]], 0.1).material('#ff8888').move(4, 0, 1)

ui.label("Head Position:")
slider_x = ui.slider(min=-150, max=150, value=0, on_change=slider_change).props('label-always')
slider_y = ui.slider(min=-150, max=150, value=0, on_change=slider_change).props('label-always')
slider_z = ui.slider(min=0, max=300, value=0, on_change=slider_change).props('label-always')

ui.label("Wand:")
with ui.row():
    knob_yaw = ui.knob(0, min=0, max=360, show_value=True, on_change=knob_change)
    knob_pitch = ui.knob(0, min=0, max=360, show_value=True, on_change=knob_change)
    knob_roll = ui.knob(0, min=0, max=360, show_value=True, on_change=knob_change)
    ui.joystick(color='blue', size=80, on_move=lambda e: update_values(e), on_end=lambda e: end_values(e))
    
with ui.row():
    button1 = ui.button('Trigger', on_click=lambda: buttonActive(1))
    button2 = ui.button('Button 1', on_click=lambda: buttonActive(2))
    button3 = ui.button('Button 2', on_click=lambda: buttonActive(4))
    button4 = ui.button('Button 3', on_click=lambda: buttonActive(8))
    button5 = ui.button('Button 4', on_click=lambda: buttonActive(16))
    button6 = ui.button('Button 5', on_click=lambda: buttonActive(32))
    button7 = ui.button('Button 6', on_click=lambda: buttonActive(64))
    button8 = ui.button('Button 7', on_click=lambda: buttonActive(128))
    button9 = ui.button('Button 8', on_click=lambda: buttonActive(256))



ui.run()
