# motor　の数字
#       3             2
#
#
#              1
#
#

class ControlValue():
    def __init__(self,topleft,bottomright,image_width, image_height):
        # self.topleft = topleft
        # self.bottomright = bottomright
        # x,y
        # ----------------->
        # |
        # |
        # |
        # |
        # V
        self.center = ((bottomright[0] + topleft[0]) //2,(bottomright[1] + topleft[1]) //2)
        self.width = image_width
        self.height = image_height
        # self.mode = 
    def set_velocity(self,rotation_velocity=10,moving_velocity=10):
        self.rotation_velocity = rotation_velocity
        self.moving_velocity = moving_velocity

    def detect_mode(self):
        center_x = self.center[0]
        width = self.width
        if center_x == -1:
            self.mode = "stop"
        elif width*4//10 <= center_x <= width*6//10:
            self.mode = "run"
        elif center_x < width*4//10:
            self.mode = "rotate_cw"
        else:
            self.mode = "rotate_ccw"
        return self.mode

    def get_control_value(self):
        if self.mode == "rotate_cw":
            r =  self.rotation_velocity
            v1 = -r
            v2 = -r
            v3 = -r
            return v1,v2,v3
        elif self.mode == "rotate_ccw":
            r =  self.rotation_velocity
            v1 = r
            v2 = r
            v3 = r
            return v1,v2,v3
        elif self.mode == "run":
            vy = self.moving_velocity
            vx = 0
            root3 = 3**(0.5)
            v1 = vx
            v2 = -vx/2 + vy*root3/2
            v3 = -vx/2 - vy*root3/2
            return v1,v2,v3
        else:
            v1 = v2 = v3 = 0
            return v1,v2,v3


