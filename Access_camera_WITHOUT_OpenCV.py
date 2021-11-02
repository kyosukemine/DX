import sys
import pygame
from PIL import Image
import pygame
import pygame.camera
from pygame.locals import *





pygame.init()
pygame.camera.init()
camlist = pygame.camera.list_cameras()
if camlist:
    cam = pygame.camera.Camera(camlist[0],(640,480))

cam.start()



size = width, height = 640, 480

screen = pygame.display.set_mode(size)

BLACK = (0, 0, 0)
ORIGIN = (0, 0)
while 1:
  for event in pygame.event.get():
    if event.type == pygame.QUIT: sys.exit()

  image = cam.get_image()
  screen.fill(BLACK)
  screen.blit(image, ORIGIN)
  pygame.display.flip()
