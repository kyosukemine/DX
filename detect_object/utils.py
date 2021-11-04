# Copyright 2021 The TensorFlow Authors. All Rights Reserved.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
"""Utility functions to display the pose detection results."""

from typing import List

from PIL import Image, ImageDraw, ImageFont, ImageOps
# import cv2
import numpy as np
from object_detector import Detection

_MARGIN = 10  # pixels
_ROW_SIZE = 10  # pixels
_FONT_SIZE = 1
_FONT_THICKNESS = 1
_TEXT_COLOR = (0, 0, 255)  # red


def visualize(
    image: np.ndarray,
    detections: List[Detection],
) -> np.ndarray:
  """Draws bounding boxes on the input image and return it.

  Args:
    image: The input RGB image.
    detections: The list of all "Detection" entities to be visualize.

  Returns:
    Image with bounding boxes.
  """
  for detection in detections:
    # Draw bounding_box
    start_point = detection.bounding_box.left, detection.bounding_box.top
    end_point = detection.bounding_box.right, detection.bounding_box.bottom
    # cv2.rectangle(image, start_point, end_point, _TEXT_COLOR, 3)
    
    
    pil_image = Image.fromarray(image)
    pil_image = ImageOps.flip(pil_image)
    draw = ImageDraw.Draw(pil_image)
    draw.rectangle((start_point, end_point),outline=_TEXT_COLOR)
    
    
    

    # Draw label and score
    category = detection.categories[0]
    class_name = category.label
    probability = round(category.score, 2)
    result_text = class_name + ' (' + str(probability) + ')'
    text_location = (_MARGIN + detection.bounding_box.left,
                     _MARGIN + _ROW_SIZE + detection.bounding_box.top)
    # cv2.putText(image, result_text, text_location, cv2.FONT_HERSHEY_PLAIN,
    #             _FONT_SIZE, _TEXT_COLOR, _FONT_THICKNESS)
    
    
    font = ImageFont.load_default()
    text = class_name + ' (' + str(probability) + ')'
    left, top = start_point
    txpos = (left, top-_FONT_SIZE-_FONT_THICKNESS//2)
    txw, txh = draw.textsize(text, font=font)
    draw.rectangle([txpos, (left+txw, top-txh)], outline=_TEXT_COLOR, fill=_TEXT_COLOR, width=_FONT_THICKNESS)
    draw.text(txpos, text, font=font, fill=(0,0,0), size=_FONT_SIZE)
    
    pil_image = ImageOps.flip(pil_image)
    image = np.array(pil_image)
    print("\n\n", class_name, start_point, end_point, end="\n\n")
    
  return image

