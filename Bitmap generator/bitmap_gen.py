# Generated Using ChatGPT and modified for my needs
#!/usr/bin/env python3

import argparse
import sys
from pathlib import Path

from PIL import Image


# Display dimensions
WIDTH = 128
HEIGHT = 64
BANDS = HEIGHT // 8


def image_to_bitmap(image_path, threshold=128, dither=False):
    """
    Convert an image to a column-packed monochrome bitmap.

    Returns:
        A list containing 8 bands, each with 128 bytes.

    Bit layout:
        Bit 0 = top pixel
        Bit 7 = bottom pixel
        0 = off
        1 = on
    """

    image = Image.open(image_path).convert("L")

    # Resize image to fit the 128x64 display.
    image = image.resize(
        (WIDTH, HEIGHT),
        Image.Resampling.LANCZOS,
    )

    # Convert the image to binary pixels.
    if dither:
        # Floyd-Steinberg dithering approximates grayscale
        # using patterns of black and white pixels.
        image = image.convert(
            "1",
            dither=Image.Dither.FLOYDSTEINBERG,
        )

        pixels = [
            [
                0 if image.getpixel((x, y)) else 1
                for x in range(WIDTH)
            ]
            for y in range(HEIGHT)
        ]

    else:
        # Simple thresholding without dithering.
        pixels = [
            [
                0 if image.getpixel((x, y)) >= threshold else 1
                for x in range(WIDTH)
            ]
            for y in range(HEIGHT)
        ]

    # Allocate the bitmap: 8 bands x 128 columns.
    bitmap = [[0] * WIDTH for _ in range(BANDS)]

    # Pack each group of 8 vertical pixels into one byte.
    for band in range(BANDS):
        for x in range(WIDTH):
            value = 0

            for bit in range(8):
                y = band * 8 + bit
                value |= pixels[y][x] << bit

            bitmap[band][x] = value

    return bitmap


def write_c_file(bitmap, output_path, array_name="bitmap"):
    """
    Write the bitmap to a C source file as a const uint8_t array.
    """

    with open(output_path, "w", encoding="utf-8") as file:
        file.write("#include <stdint.h>\n\n")
        file.write(
            f"const uint8_t {array_name}[8][128] = {{\n"
        )

        for band, row in enumerate(bitmap):
            file.write("    {\n        ")

            for x, value in enumerate(row):
                file.write(f"0x{value:02X}")

                if x < len(row) - 1:
                    file.write(", ")

                # Eight bytes per line for readability.
                if (x + 1) % 8 == 0 and x < len(row) - 1:
                    file.write("\n        ")

            file.write("\n    }")

            if band < len(bitmap) - 1:
                file.write(",")

            file.write("\n")

        file.write("};\n")


def main():
    parser = argparse.ArgumentParser(
        description=(
            "Convert an image to a 128x64 monochrome C bitmap."
        )
    )

    parser.add_argument(
        "image",
        help="Input image path",
    )

    parser.add_argument(
        "-o",
        "--output",
        default="bitmap.c",
        help="Output C file (default: bitmap.c)",
    )

    parser.add_argument(
        "-t",
        "--threshold",
        type=int,
        default=None,
        help="Black/white threshold from 0 to 255 (default: 128)",
    )

    parser.add_argument(
        "--dither",
        action="store_true",
        help="Use Floyd-Steinberg dithering",
    )

    parser.add_argument(
        "-n",
        "--name",
        default="bitmap",
        help="C array name (default: bitmap)",
    )

    args = parser.parse_args()

    # Set the default threshold.
    if args.threshold is None:
        args.threshold = 128

    # Validate threshold.
    if not 0 <= args.threshold <= 255:
        parser.error("Threshold must be between 0 and 255.")

    # Validate C array name.
    if not args.name.isidentifier():
        parser.error("Array name must be a valid C identifier.")

    # Warn if the threshold was explicitly supplied with dithering.
    if args.dither and (
        "-t" in sys.argv or "--threshold" in sys.argv
    ):
        print(
            "Warning: --threshold is ignored when --dither "
            "is enabled. Pillow's dithering conversion uses "
            "its own thresholding behavior.",
            file=sys.stderr,
        )

    try:
        bitmap = image_to_bitmap(
            args.image,
            threshold=args.threshold,
            dither=args.dither,
        )

        write_c_file(
            bitmap,
            args.output,
            array_name=args.name,
        )

    except (OSError, ValueError) as error:
        parser.error(str(error))

    print(
        f"Created {args.output}: "
        f"{WIDTH}x{HEIGHT} bitmap"
    )


if __name__ == "__main__":
    main()
