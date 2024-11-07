#!/usr/bin/env python3
import sys
from PIL import Image

def validate_image(data, width, height):
    # Create a new image
    image = Image.new('RGBA', (width, height))

    # Load the pixel data into the image
    image.putdata([(data[i], data[i + 1], data[i + 2], data[i + 3]) for i in range(0, len(data), 4)])

    # Save or display the image for visual inspection
    image.save('output_image.png')

if __name__ == '__main__':
    if len(sys.argv) != 2:
        print('Usage: ./make-image.py FILENAME')
        exit(1)
    filename = sys.argv[1]

    image = Image.open(filename)
    width, height = image.size

    print(f'Size: {width}x{height}')
    data = []
    for i in range(height):
        for j in range(width):
            p = image.getpixel((j, i))
            data.append(p[0])
            data.append(p[1])
            data.append(p[2])
            data.append(p[3])
    print('Total length: ' + str(len(data)))

    image_h = 'const unsigned char image_data[] = {\n'
    # image_h += ', '.join(map(str, data))
    for i in range(0, len(data), 4):
        line = ', '.join(map(str, data[i:i+4]))
        image_h += '    ' + line + ',\n'
    image_h += '\n};\n'

    # print(image_h)
    f = open('image.h', 'w')
    f.write(image_h)
    f.close()

    # validate_image(data, width, height)

